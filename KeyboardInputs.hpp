#include "Inputs.hpp"
#include <map>

class KeyboardInputs : public Inputs {
public :
    KeyboardInputs();
    ~KeyboardInputs();
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
