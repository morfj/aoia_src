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
            if (!instance_)
            {
                instance_ = new T;
            }
            return instance_;
        }

        static void DestroyInstance()
        {
            delete instance_;
            instance_ = NULL;
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

        static T* instance_;
    };

    /// Static class member initialisation.
    template <typename T> T* Singleton<T>::instance_ = NULL;

}   // namespace
