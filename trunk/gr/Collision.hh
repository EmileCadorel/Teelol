#ifndef _COLL
#define _COLL

#include <iostream>
#include <vector>
#include "Form.hh"
using namespace std;

enum direction_t {
  NORTH = 1, SOUTH, EAST, WEST
};


class Collision{
public:
  Collision(Form * f, vector<Form> tab);
  Collision(Form * f, Form * t);
  direction_t get_direction();

private:
  bool is_inside(Form * f, Form * t);
  direction_t m_direction;

};



#endif 