#extension GL_NV_shader_atomic_float : enable

#ifdef GL_NV_shader_atomic_float

#define ATOMIC_FLOAT_T float
#define ATOMIC_ADD_FLOAT(dst, val) atomicAdd((dst), (val))
#define ATOMIC_GET_FLOAT(val) (val)

#else

#define ATOMIC_FLOAT_T uint
#define ATOMIC_ADD_FLOAT(dst, val)
{
  uint _old = (dst);
  for (;;) {
    uint _new = floatBitsToUint(uintBitsToFloat(_old) + (val));
    uint _prev = atomicCompSwap((dst), _old, _new);
    if (_prev == _old)
      break;
    _old = _prev;
  }
}
#define ATOMIC_GET_FLOAT(val) uintBitsToFloat(val)

#endif