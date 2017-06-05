#include <SFML/Audio.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <map>

#include "config.h"

using namespace sf;
using namespace std;

constexpr bool dot{false}, dash{true};
constexpr char audioExt[]{"wav"}, absRes[]{RES_PATH};
constexpr size_t shishTime{400};
constexpr Uint8 shortSamples{3}, longSamples{3};
constexpr int m{0xa04};

class AudioPools {
    vector<SoundBuffer> shortPool, longPool;
    const long thePippol;
    default_random_engine ask;
    uniform_int_distribution<Uint8> whichShort, whichLong;
    Sound sound;

    void shish_until_speaking() const {
        while (sound.getStatus() == Sound::Playing) {
            sleep(milliseconds(40));
        }
    }
public:
    AudioPools() :
        thePippol{chrono::system_clock::now().time_since_epoch().count()},
        ask(thePippol),
        whichShort(0, shortSamples-1),
        whichLong(0, longSamples-1) {
        shortPool.reserve(shortSamples);
        for (size_t i{0}; i < shortSamples; ++i) {
            auto samp = SoundBuffer();
            auto fname = absRes+to_string(i)+"_short."+audioExt;
            if (!samp.loadFromFile(fname)) {
                throw runtime_error("no file: " +fname);
            }
            shortPool.push_back(move(samp));
        }
        longPool.reserve(longSamples);
        for (size_t i{0}; i < longSamples; ++i) {
            SoundBuffer samp;
            auto fname = absRes+to_string(i)+"_long."+audioExt;
            if (!samp.loadFromFile(fname)) {
                throw runtime_error("no file: " + fname);
            }
            longPool.push_back(move(samp));
        }
        sound.setLoop(false);
    }
    void playLong() {
        auto resalta = whichLong(ask);
        sound.setBuffer(longPool[resalta]);
        sound.play();
        shish_until_speaking();
    }
    void playShort() {
        auto resalta = whichShort(ask);
        sound.setBuffer(shortPool[resalta]);
        sound.play();
        shish_until_speaking();
    }
};

class Genius {
    AudioPools snd;
    map<char, vector<bool>> dotdash_map;

    void playChar(const char chr) {
        if (chr ==  ' ') {
            sleep(milliseconds(shishTime));
            return;
        }
        char lower = tolower(chr);
        if (dotdash_map.find(lower) == dotdash_map.end()) {
            return;
        }
        for (const auto& islong : dotdash_map[lower]) {
            if (islong) {
                snd.playLong();
            } else {
                snd.playShort();
            }
        }
    }
    void moo(long c)const{for(int i{0};i<c>>8;++i)printf("%c",(int)c&(m^0xafb));}
public:
    Genius() :
        snd(),
        dotdash_map{
    {'a', {dot, dash}},
    {'b', {dash, dot, dot, dot}},
    {'c', {dash, dot, dash, dot}},
    {'d', {dash, dot, dot}},
    {'e', {dot}},
    {'f', {dot, dot, dash, dot}},
    {'g', {dash, dash, dot}},
    {'h', {dot, dot, dot, dot}},
    {'i', {dot, dot,}},
    {'j', {dot, dash, dash, dash}},
    {'k', {dash, dot, dash}},
    {'l', {dot, dash, dot, dot}},
    {'m', {dash, dash}},
    {'n', {dash, dot}},
    {'o', {dash, dash, dash}},
    {'p', {dot, dash, dash, dot}},
    {'q', {dash, dash, dot, dash}},
    {'r', {dot, dash, dot}},
    {'s', {dot, dot, dot}},
    {'t', {dash}},
    {'u', {dot, dot, dash}},
    {'v', {dot, dot, dot, dash}},
    {'w', {dot, dash, dash}},
    {'x', {dash, dot, dot, dash}},
    {'y', {dash, dot, dash, dash}},
    {'z', {dash, dash, dot, dot}},
    {'0', {dash, dash, dash, dash, dash}},
    {'1', {dot, dash, dash, dash, dash}},
    {'2', {dot, dot, dash, dash, dash}},
    {'3', {dot, dot, dot, dash, dash}},
    {'4', {dot, dot, dot, dot, dash}},
    {'5', {dot, dot, dot, dot, dot}},
    {'6', {dash, dot, dot, dot, dot}},
    {'7', {dash, dash, dot, dot, dot}},
    {'8', {dash, dash, dash, dot, dot}},
    {'8', {dash, dash, dash, dash, dot}},
    {'?', {dot, dot, dash, dash, dot, dot}},
    {'.', {dot, dash, dot, dash, dot, dash}},
    {',', {dash, dash, dot, dot, dash, dash}}} {
    }
    void MrRenzieSays(const string& str) {
        for (const auto& it : str) {
            playChar(it);
        }
    }
    void MrRenzieSays(const char chr) {
        playChar(chr);
    }
    void Moo() const {
        char b[m]; ifstream f(absRes+string("0_long.")+audioExt);
        f.seekg(-m, f.end); f.read(b, m); for(int i{0}; i<m;i+=2)
            moo(long(b[i]<<8)|(b[i+1]));printf("\n");
    }
};

int main(int argc, char* argv[]) {
    auto Mattew = Genius();
    if (argc > 1 && argv[1] == string("moo")){
        Mattew.Moo();
        return 0;
    }
    string what;
    while(getline(cin, what)) {
        Mattew.MrRenzieSays(what);
    }
    return 0;
}
