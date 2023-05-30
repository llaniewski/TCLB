#ifndef HANDLER_FACTORY_H
#define HANDLER_FACTORY_H

#include "Factory.h"
#include "pugixml.hpp"
#include "Handlers/vHandler.h"

//typedef Factory< vHandler, pugi::xml_node > HandlerFactory;

class HandlerFactory : public Factory< vHandler, pugi::xml_node > {

};

template <class T>
vHandler * GenericAsk(const pugi::xml_node& node) {
  if (node.name() == T::xmlname) {
    return new T;
  }
  return NULL;
}

#endif