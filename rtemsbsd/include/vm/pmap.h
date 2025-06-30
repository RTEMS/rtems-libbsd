struct pmap;
typedef struct pmap *pmap_t;
void		 pmap_qremove(vm_offset_t, int);
void		 pmap_qenter(vm_offset_t, vm_page_t *, int);
