#pragma once
class MyInput {
public:
    MyInput();
    MyInput(const MyInput& other);
    virtual ~MyInput();

    void initialize();

    void keyDown(unsigned int input);
    void keyUp(unsigned int input);

    bool isKeyDown(unsigned int key);
private:
    bool m_keys[256];
};

