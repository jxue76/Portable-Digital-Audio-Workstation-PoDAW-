// This file defines the KeyboardInputs class, which implements the Inputs interface to emulate button states from a keyboard device.
#include <map>

class KeyboardInputs : public Inputs {
public :
    KeyboardInputs();
    bool isUpPressed() const override;
    bool isDownPressed() const override;
    bool isLeftPressed() const override;
    bool isRightPressed() const override;
    bool isAPressed() const override;
    bool isBPressed() const override;
    bool isXPressed() const override;
    Dial getDialPosition() const override;

private:
    int fd;
    mutable std::map<int, bool> keyStates;
    void updateKeyStates() const;
};
