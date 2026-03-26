/*
This is the top-level interface for reading inputs from the hardware. It defines the Inputs class, which provides virtual methods for checking the state of various buttons and a dial.
*/

//Defines dial states
enum class Dial {
    UP,
    DOWN,
    NEUTRAL
};

//Defines all pollable inputs: buttons (up, down, left, right, A, B, X) and a dial (up, down, neutral)
class Inputs {
protected:
    Inputs();
public:
    virtual bool isUpPressed() const;
    virtual bool isDownPressed() const;
    virtual bool isLeftPressed() const;
    virtual bool isRightPressed() const;
    virtual bool isAPressed() const;
    virtual bool isBPressed() const;
    virtual bool isXPressed() const;
    virtual Dial getDialPosition() const;
};