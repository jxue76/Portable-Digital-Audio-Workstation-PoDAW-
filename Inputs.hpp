enum class Dial {
    UP,
    DOWN,
    NEUTRAL
};
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