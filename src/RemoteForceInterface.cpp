#include "Global.h"
#include "RemoteForceInterface.hpp"
#include "pinned_allocator.hpp"

namespace rfi {
    template class RemoteForceInterface< ForceCalculator, RotParticle, ArrayOfStructures, particle_real_t, pinned_allocator<particle_real_t> >;
};