#pragma once


namespace Shared
{

    template<typename T>
    class Singleton
    {
    public:
        /// Unique point of access
        static T* Instance()
        {
            if (!ms_instance)
            {
                ms_instance = new T;
            }
            return ms_instance;
        }

        static void DestroyInstance()
        {
            delete ms_instance;
            ms_instance = NULL;
        }

    private:
        /// Prevent clients from creating a new singleton
        Singleton()
        {
        }

        /// Prevent clients from creating a copy of the singleton
        Singleton(const Singleton<T>&)
        {
        }

        static T* ms_instance;
    };

    /// Static class member initialisation.
    template <typename T> T* Singleton<T>::ms_instance = NULL;

}   // namespace
