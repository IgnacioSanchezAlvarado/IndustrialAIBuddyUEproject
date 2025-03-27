import json
import boto3
import base64
from datetime import datetime
import wave
import struct
import io
import time
from amazon_transcribe.client import TranscribeStreamingClient
from amazon_transcribe.handlers import TranscriptResultStreamHandler
from amazon_transcribe.model import TranscriptEvent
import asyncio

def get_language_code(language):
    language_mapping = {
        "English": "en-US",
        "Spanish": "es-US",
        "French": "fr-FR"
    }
    return language_mapping.get(language, "en-US")

def convert_to_pcm(raw_data):
    """
    Convert float32 raw_data to PCM signed 16-bit little-endian format
    Input: float32 array (raw_data)
    Output: PCM signed 16-bit little-endian bytes
    """
    # Unpack float32 samples
    float_samples = struct.unpack('f' * (len(raw_data) // 4), raw_data)
    
    # Convert to int16 samples
    pcm_data = bytearray()
    for sample in float_samples:
        # Clip the sample to [-1.0, 1.0]
        sample = max(min(sample, 1.0), -1.0)
        # Convert to int16 range [-32768, 32767]
        int16_sample = int(sample * 32767)
        # Pack as signed 16-bit little-endian
        pcm_data.extend(struct.pack('<h', int16_sample))
    
    return bytes(pcm_data)

class MyEventHandler(TranscriptResultStreamHandler):
    def __init__(self, stream):
        super().__init__(stream)
        self.full_transcript = []  # Change to list to store all segments
        
    async def handle_transcript_event(self, transcript_event: TranscriptEvent):
        results = transcript_event.transcript.results
        
        if not results:
            return
            
        for result in results:
            if result.alternatives:
                transcript = result.alternatives[0].transcript
                
                if not result.is_partial and transcript:
                    # Check if this transcript is already in our list
                    if not self.full_transcript or self.full_transcript[-1] != transcript:
                        self.full_transcript.append(transcript)
                        print(f"FINAL: {transcript}")

    def get_transcript(self):
        """Return the full transcript joined together"""
        return ' '.join(self.full_transcript)


async def get_streaming_transcription(audio_data, language):
    print(f"Starting transcription with audio data length: {len(audio_data)} bytes")
    
    client = TranscribeStreamingClient(region="us-east-1")
    language_code = get_language_code(language)
    
    # Log stream configuration
    print("Starting stream with configuration:")
    print("- Language: en-US")
    print("- Sample rate: 48000 Hz")
    print("- Encoding: pcm")
    
    stream = await client.start_stream_transcription(
        language_code=language_code,
        media_sample_rate_hz=48000,
        media_encoding="pcm",
        enable_partial_results_stabilization=True,
        partial_results_stability="high",
        number_of_channels=2,
        # Add these parameters
        vocabulary_name=None,
        session_id=None,
        vocab_filter_method=None,
        show_speaker_label=False,
        enable_channel_identification=True
    )

    handler = MyEventHandler(stream.output_stream)
    
    async def write_chunks():
        chunks_sent = 0
        try:
            CHUNK_SIZE = 8 * 1024  # 8KB chunks
            audio_buffer = io.BytesIO(audio_data)
            
            while True:
                chunk = audio_buffer.read(CHUNK_SIZE)
                if not chunk:
                    break
                
                # Add a small delay between chunks to prevent overwhelming the service
                await stream.input_stream.send_audio_event(audio_chunk=chunk)
                chunks_sent += 1
                await asyncio.sleep(0.01)  # Small delay between chunks
            
            print(f"Finished sending all chunks (total: {chunks_sent})")
            # Wait a bit before ending the stream
            await asyncio.sleep(0.5)
            await stream.input_stream.end_stream()
            
        except Exception as e:
            print(f"Error in write_chunks: {str(e)}")
            raise


    try:
        # Set a timeout for the entire operation
        timeout = 12  # seconds
        await asyncio.wait_for(
            asyncio.gather(write_chunks(), handler.handle_events()),
            timeout=timeout
        )
        return handler.get_transcript()
        
    except asyncio.TimeoutError:
        print(f"Transcription timed out after {timeout} seconds")
        return handler.get_transcript()
    except Exception as e:
        print(f"Error in transcription: {str(e)}")
        raise

def lambda_handler(event, context):
    start_time = time.time()

    try:
        body = json.loads(event['body'])
        audio_data = body.get('audioData')
        language = body.get('language')
        
        if not audio_data:
            raise ValueError("No audio data provided")
        
        # Decode base64
        decoded_audio = base64.b64decode(audio_data)
        print(f"Decoded base64 data length: {len(decoded_audio)} bytes")
        
        # Convert raw float32 data to PCM format
        pcm_data = convert_to_pcm(decoded_audio)
        print(f"Converted to PCM data length: {len(pcm_data)} bytes")
        
        # Get transcription using streaming
        transcript_text = asyncio.get_event_loop().run_until_complete(
            get_streaming_transcription(pcm_data, language)
        )
        
        execution_time = time.time() - start_time

        response = json.dumps({
                'transcript': transcript_text,
                'executionTime': f'{execution_time:.2f} seconds',
                'message': 'Audio processed successfully'
            })
        
        print(f"Response: {response}")
        
        return {
            'statusCode': 200,
            'body': response
        }
        
    except Exception as e:
        execution_time = time.time() - start_time
        print(f"Error: {str(e)}")
        return {
            'statusCode': 500,
            'body': json.dumps({
                'error': f'Error processing audio: {str(e)}',
                'executionTime': f'{execution_time:.2f} seconds'
            })
        }