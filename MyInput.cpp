#include "MyInput.h"


MyInput::MyInput() {
}


MyInput::~MyInput() {
}

MyInput::MyInput(const MyInput& other) {

}

void MyInput::initialize() {
    for (int i = 0; i < 256; ++i) {
        m_keys[i] = false;
    }
    return;
}

void MyInput::keyDown(unsigned int input) {
    m_keys[input] = true;
    return;
}

void MyInput::keyUp(unsigned int input) {
    m_keys[input] = false;
    return;
}

bool MyInput::isKeyDown(unsigned int key) {
    return m_keys[key];
}
