#pragma once

#include <type_traits>
#include <utility>

namespace Nexus
{
    /// @brief A class that repsents a wrapper around some kind of handle, e.g. an
    /// OpenGL texture
    /// @tparam Handle The resource that will be held by the AutoRelease
    /// @tparam Deleter The function to clean up the Handle when it goes out of scope
    /// @tparam InvalidValue The value that indicates that the Handle is invalid
    template <typename Handle, Handle InvalidValue, typename Deleter>
    class AutoRelease
    {
      public:
        /// @brief A default constructor, creates an invalid handle and no destructor
        AutoRelease() noexcept = default;

        /// @brief A constructor taking in a handle, creates a default deleter, if
        /// one is available. Only available if the deleter is default constructible.
        /// @tparam D Template of the deleter, used to deduce if the deleter is
        /// default constructible
        /// @param handle The handle to store in the AutoRelease
        template <typename D = Deleter>
            requires std::is_default_constructible_v<D>
        explicit AutoRelease(Handle handle) noexcept;

        /// @brief A constructor taking in handle of the resource and a deleter to
        /// clean it up
        /// @param handle The handle of the resource
        /// @param deleter The function to call when cleaning up the handle
        AutoRelease(Handle handle, Deleter deleter) noexcept;

        /// @brief Copy construction of this object is not supported, as there can
        /// only be one owner of the handle
        /// @param Another AutoRelease object to copy
        AutoRelease(const AutoRelease &) = delete;

        /// @brief Copy assignment of this object is not supported, as there can only
        /// be one owner of the handle
        /// @param Another AutoRelease object to copy
        AutoRelease &operator=(const AutoRelease &) = delete;

        /// @brief Move constructor, taking in an r-value reference to the object to
        /// take ownership of
        /// @param other The object to take ownership of
        AutoRelease(AutoRelease &&other) noexcept;

        /// @brief Move assignment operator, taking in an r-value reference to the
        /// object to take ownership of
        /// @param other The object to take ownership of
        AutoRelease &operator=(AutoRelease &&other) noexcept;

        /// @brief Destructor to clean up the handle, may be noexcept depending on
        /// the deleter
        ~AutoRelease();

        /// @brief A method that releases the handle from this objects ownership and
        /// returns it to the callee
        /// @return A returned handle, that is no longer owned by this object
        [[nodiscard]] Handle Release() noexcept;

        /// @brief A method that assigns this instance to another resource handle
        /// @param newHandle The handle for this instance to take ownership of
        void Reset(Handle newHandle = InvalidValue);

        /// @brief A method that returns a reference to the handle being stored
        /// within this instance
        /// @return A reference to the handle
        Handle &Get() noexcept;

        /// @brief A method that returns a const reference to the handle being stored
        /// within this instance
        /// @return A const reference to the handle
        const Handle &Get() const noexcept;

        /// @brief A method that returns a reference to the deleter being stored
        /// within this instance
        /// @return A reference to the deleter
        Deleter &GetDeleter() noexcept;

        /// @brief A method that returns a const reference to the deleter being
        /// stored within this instance
        /// @return A const reference to the deleter
        const Deleter &GetDeleter() const noexcept;

        /// @brief An explicit bool operator to check whether the handle held by this
        /// instance is valid
        explicit operator bool() const noexcept;

        /// @brief A method that checks whether the handle held by this instance is
        /// valid
        /// @return A value indicating whether the handle is valid
        bool IsValid() const noexcept;

        /// @brief A method that swaps this instance with another AutoRelease
        /// @param other A reference to the object to swap with
        void Swap(AutoRelease &other) noexcept;

        /// @brief An operator that allows accessing the handle using ->
        /// @tparam H A type to check whether the pointer operator should be included
        /// @return A pointer to the handle
        template <typename H = Handle>
            requires std::is_pointer_v<H>
        auto operator->() const noexcept;

        /// @brief An operator that allows accessing the handle as a reference
        /// @tparam H H A type to check whether the reference operator should be
        /// included
        /// @return A reference to the handle
        template <typename H = Handle>
            requires std::is_pointer_v<H>
        auto &operator*() const noexcept;

        /// @brief A function that swaps two AutoRelease objects, for compatibility
        /// with std::swap
        /// @param a The first instance of AutoRelease to swap
        /// @param b The second instance of AutoRelease to swap
        friend void swap(AutoRelease &a, AutoRelease &b) noexcept
        {
            a.Swap(b);
        }

      private:
        /// @brief A member variable that holds the handle
        Handle m_Handle = InvalidValue;

        /// @brief A deleter for the handle
        Deleter m_Deleter{};
    };
} // namespace Nexus

#include "Core/AutoRelease.inl"