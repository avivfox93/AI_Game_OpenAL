//
// Created by Aviv on 16/07/2021.
//

#include <cstdint>
#include <AL/al.h>
#include "AudioManager.h"
#include "AudioUtils.h"

bool AudioManager::Init() {
    device = alcOpenDevice(nullptr);
    if(!device)
        return false;
    context = alcCreateContext(device, nullptr);
    return alcMakeContextCurrent(context);
}

AudioBuffer& AudioManager::GetBuffer(const char *filename) {
    auto it = buffers.find(filename);
    if(it != buffers.end())
        return *it->second;
    auto* buffer = new AudioBuffer(filename);
    buffers[filename] = buffer;
    return *buffer;
}

AudioSource AudioManager::CreateSource(const Point2D& location, AudioBuffer& buffer) {
    ALuint source;
    alGenSources((ALuint)1, &source);
    return AudioSource(source,buffer,location);
}

AudioSource AudioManager::CreateSource(const Point2D& location, const char* filename) {
    return CreateSource(location, GetBuffer(filename));
}

AudioManager::~AudioManager() {
//    fprintf(stderr,"Killing Audio Manager\n");
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

AudioSource::~AudioSource() {
//    fprintf(stderr,"Killing Audio Source\n");
    alDeleteSources(1, &channel);
}

AudioSource::AudioSource(ALuint channel, AudioBuffer &audioBuffer, const Point2D &location) : channel(channel),
                                                                                              location(location) {
    alSourcei(channel,AL_BUFFER,audioBuffer.GetBuffer());
    alSourcef(channel, AL_PITCH, 1.0f);
    alSourcef(channel, AL_GAIN, 1.0f);
    alSource3f(channel, AL_POSITION, 0, 0, 0);
    alSource3f(channel, AL_VELOCITY, 0, 0, 0);
    alSourcei(channel, AL_LOOPING, AL_FALSE);
}

void AudioSource::UpdateLocation(const Point2D &location) {
    this->location = location;
    alSource3f(channel, AL_POSITION, location.x, location.y, 0);
}

void AudioSource::Play() const {
    alSourcePlay(channel);
}

void AudioSource::Stop() const {
    alSourceStop(channel);
}

bool AudioSource::Playing() const {
    ALint state;
    alGetSourcei(channel,AL_SOURCE_STATE,&state);
    return state == AL_PLAYING;
}

void AudioSource::SetVolume(float volume) const {
    alSourcef(channel, AL_GAIN, volume);
}

AudioBuffer::AudioBuffer(const char* filename) {
//    fprintf(stderr,"Loading File %s\n",filename);
    buffer = AudioUtils::LoadWavFile(filename);
//    fprintf(stderr,"File %s Loaded?\n",filename);

}

AudioBuffer::~AudioBuffer() {
//    fprintf(stderr,"Killing Audio Buffer\n");
    alDeleteBuffers(1, &buffer);
}
