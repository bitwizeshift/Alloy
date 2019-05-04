# Extra: Use discrete GPU

**Alloy** bundles the target `Alloy::DiscreteGPU`, which can force devices
running with multiple GPUs, such as laptops, to force using the discrete
GPU over integrated graphics.

The library must be statically linked into the target executable in order
for it to be used.