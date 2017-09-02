#ifndef _SHARE_MEMORY_h_
#define _SHARE_MEMORY_h_ shareMemory.h

#include <unistd.h>  
#include <fcntl.h>  
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>  
#include <stdexcept>

template<typename _T>
class ShareMemory
{
public:
	ShareMemory(const char* name, bool create = false)
    {
        int fd;
        if (create)
        {
            fd = shm_open(name, O_RDWR | O_CREAT, 0666);  
            if (fd < 0)
            {  
                throw std::runtime_error("Can't create file mapping.");
            }  

            ftruncate(fd, sizeof(_T));  
            m_SharedMemory = (_T*)mmap(NULL, sizeof(_T), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        }
        else
        {
            fd = shm_open(name, O_RDWR, 0);  
            if (fd < 0)
            {  
                throw std::runtime_error("Can't create file mapping.");
            }  
            struct stat fileStat;
            fstat(fd, &fileStat);
  
            m_SharedMemory = (_T*)mmap(NULL, fileStat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);  
        }
        close(fd);	
    }

	~ShareMemory()
	{
        //memory will be unmapped when process is terminated.
	}

	_T* get() const
	{
	    return m_SharedMemory;
	}
	
	_T* operator->() const
	{
	    return m_SharedMemory;
	}
private:
	_T* m_SharedMemory;
};

#endif//_SHARE_MEMORY_h_
