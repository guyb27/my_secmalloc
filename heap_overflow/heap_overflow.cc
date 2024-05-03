#include <iostream>
#include <fstream>
#include <string.h>

class IFile
{
    virtual void treatment(char *st) = 0;
};

class File: public IFile
{
    char    buf[20];
    public:
    virtual void treatment(char *st)
    {
        std::cout << "call to Basic::method()" << std::endl;
        std::cout << "we are here " << this << std::endl;
        std::cout << "treat File " << st << std::endl;
        std::ifstream fin;
        fin.open(st);
        if (fin.fail())
        {
            std::cout << "File could'nt open " << std::endl;
        }
        else
        {
            char sz_data;
            fin.read(&sz_data, sizeof(char));
            std::cout << "NB ITEM " << std::dec << (int)sz_data << std::endl;
            fin.read(this->buf, sz_data);
        }
    }
};

extern "C" {

    void magic_fun()
    {
        std::cout << "magically invokated" << std::endl;
    }

    struct {
        void (*fake_fun)();
    } fake_vtable[] = { magic_fun };
}

int main(int ac, char **av)
{
    static void (*m)() = &magic_fun;
    std::cout << "BEGIN" << std::endl;
    // show adr of magic
    std::cout << "magic_fun " << std::hex << *(long long*) &m << std::endl;
    std::cout << "fake_vptr " << std::hex << fake_vtable << std::endl;
    std::cout << "fake_vptr[0] " << std::hex << *(long long*)&fake_vtable[0] << std::endl;
    // instanciate File on heap
    File *inst = new File[ac];
    for (int i = 1; i < ac; i += 1)
    {
        std::cout << "-----------------------" << std::endl;
        std::cout << "Arg #" << i << std::endl;
        // show where the value of vptr
        std::cout << "vptr " << std::hex << *(long long*) &inst[i] << std::endl;
        // call on parameter
        inst[i].treatment(av[i]);
    }
    delete [] inst;
}
