#include <alsa/asoundlib.h>
#include <iostream>

int main() {
    snd_rawmidi_t* midi_in;
    const char* device = "hw:1,0,0";  // adjust from amidi -l

    if (snd_rawmidi_open(&midi_in, NULL, device, 0) < 0) {
        std::cerr << "Error opening MIDI device\n";
        return 1;
    }

    unsigned char buffer[3];

    while (true) {
        int n = snd_rawmidi_read(midi_in, buffer, sizeof(buffer));
        if (n > 0) {
	        int check_val = (int) buffer[0];
	        if (check_val  == 248 || check_val == 254)
		        continue;
            std::cout << "MIDI: ";
            for (int i = 0; i < n; i++)
               std::cout << (int)buffer[i] << " ";
            std::cout << std::endl;
           }
    }

    snd_rawmidi_close(midi_in);
    return 0;
}