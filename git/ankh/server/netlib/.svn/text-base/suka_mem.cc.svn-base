#include <cstdio>

#include <l4/re/util/object_registry>
#include <l4/re/dataspace>      // L4Re::Dataspace


extern "C" int suka_malloc(int size,unsigned int *virt,
             unsigned int *phys,unsigned int *handle)
{
  L4::Cap<L4Re::Dataspace> *ds = new L4::Cap<L4Re::Dataspace>;

  *ds = L4Re::Util::cap_alloc.alloc<L4Re::Dataspace>();
  if (!(*ds).is_valid())
  {
    printf("Dataspace allocation failed.\n");
    return -1;
  }

  int err =  L4Re::Env::env()->mem_alloc()->alloc(size, *ds,
                L4Re::Mem_alloc::Continuous | L4Re::Mem_alloc::Pinned);
  if (err < 0)
  {
    printf("mem_alloc->alloc() failed.\n");
    L4Re::Util::cap_alloc.free(*ds);
    return -1;
  }

  /*
   * Attach DS to local address space
   */
  l4_addr_t addr = 0;
  err = L4Re::Env::env()->rm()->attach(&addr, (*ds)->size(),
                                        L4Re::Rm::Search_addr,
                                        *ds);
  if (err < 0)
  {
    printf("Error attaching data space: %s\n", l4sys_errtostr(err));
    L4Re::Env::env()->mem_alloc()->free(*ds);
    L4Re::Util::cap_alloc.free(*ds);
    return -1;
  }

  l4_addr_t phys_addr;
  l4_size_t phys_size = size;

  err = (*ds)->phys(0,phys_addr,phys_size);
  if(err < 0)
  {
    printf("ds->phys() error %s\n", l4sys_errtostr(err));
    L4Re::Env::env()->rm()->detach((l4_addr_t)addr,&(*ds));
    L4Re::Env::env()->mem_alloc()->free(*ds);
    L4Re::Util::cap_alloc.free(*ds);
    return -1;
  }
  *virt = (unsigned int)addr;
  *phys = (unsigned int)phys_addr;
  *handle = (unsigned int)ds;
  //printf("suka_malloc(size = %x,*virt = 0x%x, *phys = 0x%x) phys_size = 0x%x\n",
  //  size,*virt,*phys,phys_size);
  return 0;
}

extern "C" int suka_free(void *virt_addr)
{
  int r;
  L4::Cap<L4Re::Dataspace> ds;

  /* Detach memory from our address space */
  if ((r = L4Re::Env::env()->rm()->detach(virt_addr, &ds)))
  {
    printf("detach() error %s\n", l4sys_errtostr(r));
    return r;
  }
  /* Free memory at our memory allocator, this is optional */
  if ((r = L4Re::Env::env()->mem_alloc()->free(ds)))
  {
    printf("free() error %s\n", l4sys_errtostr(r));
    return r;
  }

  /* Release and return capability slot to allocator */
  L4Re::Util::cap_alloc.free(ds, L4Re::Env::env()->task().cap());

  /* All went ok */
  return 0;
}

