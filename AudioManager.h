//
// Created by Aviv on 16/07/2021.
//

#ifndef TEST_AUDIOMANAGER_H
#define TEST_AUDIOMANAGER_H

#include <AL/alc.h>
#include <AL/al.h>
#include <map>
#include <string>
#include "Point2D.h"

class AudioSource;
class AudioBuffer;

class AudioManager {
public:
    static AudioManager& GetInstance(){
        static AudioManager* instance = nullptr;
        if(!instance)
            instance = new AudioManager();
        return *instance;
    }
    static AudioSource CreateSource(const Point2D& location, AudioBuffer& buffer);
    AudioSource CreateSource(const Point2D& location, const char* filename);
    bool Init();
    AudioBuffer& GetBuffer(const char* filename);
    ~AudioManager();
private:
    ALCdevice* device;
    ALCcontext* context;
    std::map<std::string,AudioBuffer*> buffers;
};

class AudioSource{
public:
    ~AudioSource();
    void UpdateLocation(const Point2D& location);
    void Play() const;
    void Stop() const;
    bool Playing()const;
    void SetVolume(float volume) const;
private:
    AudioSource(ALuint channel, AudioBuffer& audioBuffer, const Point2D& location);
    friend AudioManager;
    ALuint channel;
    Point2D location;
};

class AudioBuffer{
public:
    explicit AudioBuffer(const char* filename);
    ~AudioBuffer();
    ALuint GetBuffer() const{return buffer;}
private:
    ALuint buffer;
};

#endif //TEST_AUDIOMANAGER_H
