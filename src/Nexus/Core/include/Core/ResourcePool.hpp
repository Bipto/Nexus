#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "Core/ResourceDeclaration.hpp"

namespace Nexus
{
    template <typename T, typename Handle> class ResourcePool;

    /// @brief A class representing a unique handle to a resource in a resource pool
    /// @tparam Tag A template type to customise the resource pointed to by the
    /// handle
    template <typename Tag> struct HandleT
    {
      public:
        /// @brief A default constructor to initialise an empty object
        HandleT() = default;
        /// @brief The constructor of a resource handle
        /// @param index The index of the resource in a resource pool
        /// @param generation The generation of the resource in the pool (slots can
        /// be reused multiple times)
        /// @param parent The parent resource pool of the handle
        HandleT(uint32_t index, uint32_t generation, void *parent);

        /// @brief A method to return the index of the handle
        /// @return An integer representing the index of the handle
        uint32_t GetIndex() const;

        /// @brief A method to return the generation of the handle
        /// @return An integer representing the generation of the handle
        uint32_t GetGeneration() const;

        /// @brief A method that returns a pointer to the parent object of this
        /// handle
        /// @return A pointer to the parent object
        void *GetParent() const;

        /// @brief A defaulted spaceship operator to handle comparisons between
        /// handles
        /// @param A const reference to a handle to compare against
        /// @return A comparison type to compare the handles
        auto operator<=>(const HandleT &) const = default;

      private:
        /// @brief An integer containing the index of the handle
        uint32_t m_Index = 0;

        /// @brief An integer containing the generation of the handle
        uint32_t m_Generation = 0;

        /// @brief A pointer to the parent resource pool of the handle
        void *m_Parent = nullptr;
    };

    /// @brief A class representing a handle into a ResourcePool that can be owned by
    /// multiple objects
    /// @tparam T The resource that is pointed to by the handle
    /// @tparam Handle The handle that references the resource
    template <typename T, typename Handle> class SharedHandle
    {
      public:
        /// @brief A default constructor to create an empty shared handle
        SharedHandle() = default;

        /// @brief A constructor creating a shared handle from a resource pool and a
        /// resource handle
        /// @param pool A pointer to the resource pool that the handle was created
        /// from
        /// @param handle A handle to the resource within the pool
        SharedHandle(ResourcePool<T, Handle> *pool, Handle handle);

        /// @brief A destructor to clean up any resources
        ~SharedHandle() = default;

        /// @brief An operator allowing access to the resource as a pointer
        /// @return A pointer to the resource
        T *operator->();

        /// @brief An operator allowing access to the resource as a const pointer
        /// @return A pointer to the resource
        const T *operator->() const;

        /// @brief An operator allowing de-referencing the resource
        /// @return A reference to the resource
        T &operator*();

        /// @brief An operator allowing de-referencing the resource
        /// @return A reference to the resource
        const T &operator*() const;

        /// @brief A function that checks whether the handle is still valid within
        /// the pool
        /// @return A boolean indicating that the resource is still alive
        bool IsValid() const;

        /// @brief A function that returns the raw handle
        /// @return The raw handle referencing the resource
        Handle Raw() const;

        /// @brief A function that returns the underlying resource
        /// @return The raw handle to the resource
        T *GetResource();

        /// @brief A const function that returns the underlying resource
        /// @return The raw handle to the resource
        const T *GetResource() const;

        /// @brief A function that returns a resource as a derived type
        /// @tparam Derived The derived type to return
        /// @return A pointer to the derived type
        template <typename Derived> Derived *AsDerived();

        /// @brief A function that returns a resource as a derived type
        /// @tparam Derived The derived type to return
        /// @return A pointer to the derived type
        template <typename Derived> const Derived *AsDerived() const;

        /// @brief A defaulted spaceship operator to handle comparisons between
        /// handles
        /// @param A const reference to a handle to compare against
        /// @return A comparison type to compare the handles
        auto operator<=>(const SharedHandle &) const = default;

      private:
        /// @brief The resource pool containing the resource
        ResourcePool<T, Handle> *m_Pool = nullptr;

        /// @brief A handle referencing the resource
        Handle m_Handle{};

        /// @brief A shared pointer acting as the control block for the resource
        std::shared_ptr<void> m_Control{};
    };

    /// @brief
    /// @tparam T
    /// @tparam Handle
    template <typename T, typename Handle> class UniqueHandle
    {
        /// @brief
      public:
        UniqueHandle() = default;

        /// @brief
        /// @param pool
        /// @param handle
        UniqueHandle(ResourcePool<T, Handle> *pool, Handle handle);

        /// @brief A move constructor allowing moving ownership of a resource to
        /// another owner
        /// @param other The resource that should be moved into this resource
        UniqueHandle(UniqueHandle &&other) noexcept;

        // Move assignment A move assignment operator allowing this resource to take
        // ownership of another UniqueHandle
        UniqueHandle &operator=(UniqueHandle &&other) noexcept;

        /// @brief A deleted copy constructor
        /// @param The resource to copy
        UniqueHandle(const UniqueHandle &) = delete;

        /// @brief A deleted copy assignment operator
        /// @param The resource to copy
        /// @return A reference to the resource that has been assigned to
        UniqueHandle &operator=(const UniqueHandle &) = delete;

        /// @brief A destructor to clean up any resources
        ~UniqueHandle();

        /// @brief A function that resets the contained resources
        void Reset();

        /// @brief An operator allowing access to the resource as a pointer
        /// @return A pointer to the underlying resource
        T *operator->();

        /// @brief An operator allowing access to the resource as a const pointer
        /// @return A pointer to the underlying resource
        const T *operator->() const;

        /// @brief An operator allowing de-referencing the resource
        /// @return A reference to the underlying object
        T &operator*();

        /// @brief An operator allowing de-referencing the resource
        /// @return A reference to the underlying object
        const T &operator*() const;

        /// @brief A function allowing checking whether the underlying resource is
        /// still valid within the pool
        /// @return A boolean indicating whether the resource is still alive
        bool IsValid() const;

        /// @brief A function that returns the raw resource handle
        /// @return The resource handle referencing the underlying resource
        Handle Raw() const;

        /// @brief A function that returns the underlying resource
        /// @return The raw handle to the resource
        T *GetResource();

        /// @brief A const function that returns the underlying resource
        /// @return The raw handle to the resource
        const T *GetResource() const;

        /// @brief A function that returns a resource as a derived type
        /// @tparam Derived The derived type to return
        /// @return A pointer to the derived type
        template <typename Derived> Derived *AsDerived();

        /// @brief A function that returns a resource as a derived type
        /// @tparam Derived The derived type to return
        /// @return A pointer to the derived type
        template <typename Derived> const Derived *AsDerived() const;

        /// @brief A defaulted spaceship operator to handle comparisons between
        /// handles
        /// @param A const reference to a handle to compare against
        /// @return A comparison type to compare the handles
        auto operator<=>(const UniqueHandle &) const = default;

      private:
        /// @brief The resource pool that contains the resource
        ResourcePool<T, Handle> *m_Pool = nullptr;

        /// @brief A handle referencing the underlying resource
        Handle m_Handle{};
    };

    /// @brief A class representing a collection of resources, identified by handles
    /// @tparam T A template parameter of the resource to store in the pool
    /// @tparam Handle A template type of a handle type to identify the resource
    template <typename T, typename Handle> class ResourcePool
    {
      public:
        /// @brief A using statement to simplify creating a unique handle
        using Unique = UniqueHandle<T, Handle>;

        /// @brief A using statement to simplify creating a shared handle
        using Shared = SharedHandle<T, Handle>;

      public:
        /// @brief A structure representing a stored resource within the pool
        struct Entry
        {
            /// @brief A template type of the resource
            std::unique_ptr<T> resource = nullptr;

            /// @brief An integer representing the generation that the resource
            /// belongs to
            uint32_t generation = 0;

            /// @brief A boolean indicating whether this resource entry is available
            bool alive = false;
        };

        /// @brief A method that enters an existing resource into the resource pool
        /// @param resource A reference to the resource to copy into the pool
        /// @return A handle representing the stored resource
        Handle Create(std::unique_ptr<T> resource);

        /// @brief A method that constructs a resource in-place within the pool
        /// @tparam ...Args A template of the function parameter types
        /// @param ...args The parameter values to be use to construct the object
        /// @return A handle representing the stored resource
        template <typename... Args> Handle Emplace(Args &&...args);

        /// @brief A method that releases an object from within the resource pool
        /// @param handle The handle of the resource to be freed
        void Destroy(Handle handle);

        /// @brief A method that creates a shared handle from an existing resource
        /// @param resource The resource that should be entered into the resource
        /// pool
        /// @return A shared handle to the resource
        SharedHandle<T, Handle> CreateShared(std::unique_ptr<T> resource);

        /// @brief A method that creates a shared handle inline from forwarded
        /// arguments
        /// @tparam ...Args The types of the variables passed into the constructor
        /// @param ...args The values of the variables passed into the constructor
        /// @return A shared handle to the resource
        template <typename... Args> SharedHandle<T, Handle> EmplaceShared(Args &&...args);

        /// @brief A method that creates a unique handle from an existing resource
        /// @param resource The resource that should be entered into the resource
        /// pool
        /// @return A unique handle to the resource
        UniqueHandle<T, Handle> CreateUnique(std::unique_ptr<T> resource);

        /// @brief A method that creates a unique handle inline from forwarded
        /// arguments
        /// @tparam ...Args The types of the variables passed into the constructor
        /// @param ...args The values of the variables passed into the constructor
        /// @return A unique handle to the resource
        template <typename... Args> UniqueHandle<T, Handle> EmplaceUnique(Args &&...args);

        /// @brief A method that retrieves a pointer to the resource from within the
        /// resource pool
        /// @param handle The handle of the resource to be retrieved
        /// @return A pointer to the underlying resource
        T *Get(Handle handle);

      private:
        /// @brief A vector containing all stored entries within the resource pool
        std::vector<Entry> m_Entries = {};

        /// @brief A vector containing all resource slots that have been marked as
        /// avaible for re-use
        std::vector<uint64_t> m_FreeList = {};
    };
} // namespace Nexus

#include "Core/ResourcePool.inl"