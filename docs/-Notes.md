### Execution Model
- Vulkan exposes one or more *devices*, each of which exposes one or more *queues*, which **may** process work asynchronously to one another
- Vulkan queues provide an interface to the execution engine of a device
	- Commands are recorded into buffers
	- Buffers are submitted to queues

### Object Model
- *Dispatchable* handles are pointers to opaque types
- *Non-dispatchable* handles are 64-bit integers whose meaning is implementation-dependent.
- ``vkCreate*`` must eventually be followed by `vkDestroy*`
- `vkAllocate` must eventually be followed by `vkDelete`

