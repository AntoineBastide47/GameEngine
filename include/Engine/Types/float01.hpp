//
// float01.h
// Author: Antoine Bastide
// Date: 14/12/2024
//

#ifndef FLOAT01_H
#define FLOAT01_H

namespace Engine {
  class float01 {
    public:
      /// Default constructor, sets the value to 0
      float01();
      /// Sets the value to the given value clamped between 0 and 1
      explicit float01(float value);

      /// Explicit conversion to a float
      operator float() const {
        return value;
      }

      /// Assignment operator
      float01 &operator=(float value);
      /// Addition assignment operator
      float01 &operator+=(float value);
      /// Subtraction assignment operator
      float01 &operator-=(float value);
      /// Multiplication assignment operator
      float01 &operator*=(float value);
      /// Division assignment operator
      float01 &operator/=(float value);
    private:
      float value;
  };
}

#endif //FLOAT01_H
