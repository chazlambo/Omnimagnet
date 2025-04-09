#include <iostream>
#include "rect.hpp"

// No return type for constructor
Rectangle::Rectangle(int x, int y) : width(x), height(y) {}
Rectangle::Rectangle(){
}
void Rectangle::set_values (int x, int y) {
  width = x;
  height = y;
}