#include "RemoteForceInterface.hpp"

#include "Global.h"
#include "pinned_allocator.hpp"

namespace rfi {
template class RemoteForceInterface<ForceCalculator, RotParticle, ArrayOfStructures, real_t, pinned_allocator<real_t> >;
};