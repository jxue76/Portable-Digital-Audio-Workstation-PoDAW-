#include "MidiRecording.hpp"
#include "MidiMessage.hpp"
#include "Instrument.hpp"
#include <algorithm>

namespace MidiUtils {

    enum class NoteGap : char {
        None = 'A',
        MissingNoteOff = 'B',
        MissingNoteOn = 'C'
    };

    // In-place addition of an offset to all event timestamps in a recording, and adjustment of the recording length accordingly
    void addOffset(MidiRecording& recording, std::chrono::microseconds offset) {
        std::vector<TimedMidiMessage>& events = recording.getEvents();
        for (auto& event : events) {
            event.setTimestamp(event.getTimestamp() + offset);
        }
        recording.setLength(recording.getLength() + offset);
    }

    // In-place insertion of an event into a recording, does not sort the recording after insertion
    void insertEvent(MidiRecording& recording, const MidiMessage& msg, std::chrono::microseconds timestamp) {
        recording.getEvents().emplace_back(msg, timestamp);
    }

    // In-place sorting of recording events by timestamp
    void sortRecording(MidiRecording& recording) {
        std::sort(recording.getEvents().begin(), recording.getEvents().end(),
            [](const TimedMidiMessage& a, const TimedMidiMessage& b) {
                return a.getTimestamp() < b.getTimestamp();
            });
    }

    // In-place filling of gaps with missing note-on and note-off events filled in to ensure all notes are properly terminated
    void fillGaps(MidiRecording& recording) {
        sortRecording(recording);
        NoteGap notegaps[255] = {NoteGap::None};
        std::vector<TimedMidiMessage>& events = recording.getEvents();
        for (const auto& event : events) {
            int midiNote = event.getNote().getMidiNote();
            if (notegaps[midiNote] == NoteGap::None) {
                if (event.isOn()) {
                    notegaps[midiNote] = NoteGap::MissingNoteOff;
                } else {
                    notegaps[midiNote] = NoteGap::MissingNoteOn;
                }
            } else if (notegaps[midiNote] == NoteGap::MissingNoteOff) {
                if (event.isOn()) {
                    notegaps[midiNote] = NoteGap::MissingNoteOff;
                } else {
                    notegaps[midiNote] = NoteGap::None;
                }
            } else if (notegaps[midiNote] == NoteGap::MissingNoteOn) {
                if (event.isOn()) {
                    notegaps[midiNote] = NoteGap::None;
                } else {
                    notegaps[midiNote] = NoteGap::MissingNoteOn;
                }
            }
        }
        for (int i = 0; i < 255; ++i) {
            if (notegaps[i] == NoteGap::MissingNoteOff) {
                events.emplace_back(MidiMessage(Note(i, 1.0f), false), recording.getLength());
            } else if (notegaps[i] == NoteGap::MissingNoteOn) {
                events.emplace_back(MidiMessage(Note(i, 1.0f), true), std::chrono::microseconds(0));
            }
        }
        sortRecording(recording);
        //return recording;
    }
    
    // In-place concatenation of two recordings, appending the source recording to the end of the destination recording, and modifying the timestamps of the source events to be relative to the end of the destination recording
    void concatenateRecordings(MidiRecording& destination, const MidiRecording& source) {
        std::vector<TimedMidiMessage>& destEvents = destination.getEvents();
        const std::vector<TimedMidiMessage>& sourceEvents = source.getEvents();
        std::chrono::microseconds sourceLength = source.getLength();
        for (const auto& event : sourceEvents) {
            destEvents.emplace_back(event, event.getTimestamp() + destination.getLength());
        }
        destination.setLength(destination.getLength() + sourceLength);
    }

    // Creates a new recording containing only the events from the source recording that fall within the specified start and end times, with timestamps adjusted to be relative to the start time of the new recording
    MidiRecording subRecording(const MidiRecording& source, std::chrono::microseconds startTime, std::chrono::microseconds endTime) {
        MidiRecording destination;
        const std::vector<TimedMidiMessage>& sourceEvents = source.getEvents();
        for (const auto& event : sourceEvents) {
            if (event.getTimestamp() >= startTime && event.getTimestamp() <= endTime) {
                destination.getEvents().emplace_back(event, event.getTimestamp() - startTime);
            }
        }
        destination.setLength(endTime - startTime);
        return destination;
    }

    // In-place overwrites of a section of the destination recording with the source recording, starting at the specified start time, and filling in any gaps with missing note-on and note-off events as necessary to ensure all notes are properly terminated
    void overwriteSection(MidiRecording& destination, const MidiRecording& source, std::chrono::microseconds startTime) {
        std::vector<TimedMidiMessage>& destEvents = destination.getEvents();
        const std::vector<TimedMidiMessage>& sourceEvents = source.getEvents();
        std::chrono::microseconds sourceLength = source.getLength();
        std::chrono::microseconds sourceEndTime = startTime + sourceLength;
        std::chrono::microseconds endTime = std::max(startTime + sourceLength, destination.getLength());
        MidiRecording destinationBefore = subRecording(destination, std::chrono::microseconds(0), startTime);
        MidiRecording destinationAfter = subRecording(destination, sourceEndTime, endTime);
        fillGaps(destinationBefore);
        fillGaps(destinationAfter);
        destination = destinationBefore;
        concatenateRecordings(destination, source);
        concatenateRecordings(destination, destinationAfter);
    }
}