#ifndef CUSTOM_H_
#define CUSTOM_H_

#define HALF 0.5
extern const float32_t HANN[256];

#define CG_BEFORE_NODE_EXECUTION(ID) \
  printf("%d\n",ID);

#endif 