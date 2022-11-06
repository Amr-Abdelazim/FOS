#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//2022: NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)


uint32 heap_space[(KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE];
uint32 cur_ptr=0,size_heap_space=(KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE;

void* kmalloc(unsigned int size)
{
	//cprintf("%d   %d\n",size_heap_space,size);
	//TODO: [PROJECT 2022 - [1] Kernel Heap] kmalloc()
	// Write your code here, remove the panic and write your code

	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	//NOTE: Allocation using NEXTFIT strategy
	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details


	//TODO: [PROJECT 2022 - BONUS1] Implement a Kernel allocation strategy
	// Instead of the Next allocation/deallocation, implement
	// BEST FIT strategy
	// use "isKHeapPlacementStrategyBESTFIT() ..."
	// and "isKHeapPlacementStrategyNEXTFIT() ..."
	//functions to check the current strategy
	//change this "return" according to your answer
	size = (size+PAGE_SIZE-1)/PAGE_SIZE;
	bool cycle=0,first_ptr=cur_ptr,cnt=0;
	void *ans=NULL;

	while(!(cycle && first_ptr==cur_ptr)){
		if(heap_space[cur_ptr])cnt=0;
		else cnt++;
		if(cnt == size){

			cur_ptr=((cur_ptr-(size-1))%size_heap_space+size_heap_space)%size_heap_space;

			for(int i=0;i<size;i++){
				if(i==0)ans=(void*)(cur_ptr*PAGE_SIZE+KERNEL_HEAP_START);
				heap_space[cur_ptr]=size;
				struct Frame_Info* frame=NULL;
				int ret=allocate_frame(&frame);

				if(ret!=E_NO_MEM){
					ret = map_frame(ptr_page_directory,frame,(void*)(cur_ptr*PAGE_SIZE+KERNEL_HEAP_START),PERM_WRITEABLE);
				}
				cur_ptr=(cur_ptr+1)%size_heap_space;
			}
			return ans;
		}
		cur_ptr=(cur_ptr+1)%size_heap_space;
		cycle = 1;
	}

	return NULL;


}


void kfree(void* virtual_address)
{
	//TODO: [PROJECT 2022 - [2] Kernel Heap] kfree()
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

	uint32 ptr=(uint32)virtual_address;
	ptr-=KERNEL_HEAP_START;
	ptr/=PAGE_SIZE;
	if(!heap_space[ptr])return;
	int size=heap_space[ptr];
	for(int i=0;i<size;i++){
		heap_space[ptr]=0;
		uint32* ptr_page_table=NULL;
		struct Frame_Info *frame=NULL;
		frame=get_frame_info(ptr_page_directory,(void*)(ptr*PAGE_SIZE+KERNEL_HEAP_START),&ptr_page_table);
		free_frame(frame);
		unmap_frame(ptr_page_directory,(void*)(ptr*PAGE_SIZE+KERNEL_HEAP_START));
		ptr=(ptr+1)%size_heap_space;
	}


}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2022 - [3] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer

	int ptr=0;
	while(ptr<size_heap_space){
		if(!heap_space[ptr]){ptr++;continue;}
		uint32* ptr_page_table=NULL;
		struct Frame_Info *frame=NULL;
		uint32 va=(ptr*PAGE_SIZE+KERNEL_HEAP_START);
		frame=get_frame_info(ptr_page_directory,(void*)va,&ptr_page_table);
		if(((ptr_page_table[PTX(va)])&0xFFFFF000)==physical_address){
			return (ptr*PAGE_SIZE+KERNEL_HEAP_START);
		}
		ptr++;
	}
	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2022 - [4] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer


	uint32* ptr_page_table = NULL;
	struct Frame_Info *frame = NULL;
	frame=get_frame_info(ptr_page_directory,(void*)virtual_address,&ptr_page_table);
	return (ptr_page_table[PTX(virtual_address)])&0xFFFFF000;
}

