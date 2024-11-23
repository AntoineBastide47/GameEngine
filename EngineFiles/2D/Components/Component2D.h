//
// IComponent2D.h
// Author: Antoine Bastide
// Date: 22/11/2024
//

#ifndef ICOMPONENT2D_H
#define ICOMPONENT2D_H

namespace Engine2D {
  /**
   * Component2D is the base class for all 2D components linked to a 2D Entity (it works like an interface in other languages).
   * If you need to create a component and want to link it to an Entity2D using Entity2D::AddComponent(), your component will
   * need to inherit from this class for it to work.
   */
  class Component2D {};
}

#endif //ICOMPONENT2D_H
