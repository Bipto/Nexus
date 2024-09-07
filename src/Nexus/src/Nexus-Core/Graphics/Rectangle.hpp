#pragma once

#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics {
/// @brief A struct representing a rectangle with a position, width and height
template <typename T> struct Rectangle {
public:
  Rectangle() = default;

  Rectangle(const Point2D<T> &position, const Point2D<T> &size)
      : m_X(position.X), m_Y(position.Y), m_Width(size.X), m_Height(size.Y) {}

  Rectangle(T x, T y, T width, T height)
      : m_X(x), m_Y(y), m_Width(width), m_Height(height) {}

  void SetX(T x) { m_X = x; }
  void SetY(T y) { m_Y = y; }

  void SetWidth(T width) { m_Width = width; }

  void SetHeight(T height) { m_Height = height; }

  const T GetLeft() const { return m_X; }

  const T GetRight() const { return m_X + m_Width; }

  const T GetTop() const { return m_Y; }

  const T GetBottom() const { return m_Y + m_Height; }

  const T GetHeight() const { return m_Height; }

  const T GetWidth() const { return m_Width; }

  const bool Contains(const Nexus::Point2D<T> &other) const {
    return other.X >= this->m_X && other.X < this->m_X + this->m_Width &&
           other.Y >= this->m_Y && other.Y < this->m_Y + this->m_Height;
  }

  const bool Contains(const Rectangle<T> &other) const {
    return this->GetRight() <= other.GetLeft() &&
           other.GetRight() <= this->GetRight() &&
           this->GetTop() <= other.GetTop() &&
           other.GetBottom() <= this->GetBottom();
  }

  const bool Intersects(const Rectangle &other) const {
    return other.GetRight() < this->GetRight() &&
           this->GetLeft() < other.GetRight() &&
           other.GetTop() < this->GetBottom() &&
           this->GetTop() < other.GetBottom();
  }

  static Rectangle Union(const Rectangle &rect1, const Rectangle &rect2) {
    T x = glm::min(rect1.GetLeft(), rect2.GetLeft());
    T y = glm::min(rect1.GetTop(), rect2.GetTop());
    T width = glm::max(rect1.GetRight(), rect2.GetRight()) - x;
    T height = glm::max(rect1.GetBottom(), rect2.GetBottom()) - y;

    return Rectangle(x, y, width, height);
  }

  void Inflate(T horizontalAmount, T verticalAmount) {
    m_X -= horizontalAmount;
    m_Y -= verticalAmount;
    m_Width += horizontalAmount * 2;
    m_Height += verticalAmount * 2;
  }

  template <typename Other> const Rectangle To() const {
    return {(Other)GetLeft(), (Other)GetTop(), (Other)GetWidth(),
            (Other)GetHeight()};
  }

  void Offset(T x, T y) {
    m_X += x;
    m_Y += y;
  }

  const void Deconstruct(T *x, T *y, T *width, T *height) const {
    if (x) {
      *x = m_X;
    }

    if (y) {
      *y = m_Y;
    }

    if (width) {
      *width = m_Width;
    }

    if (height) {
      *height = m_Height;
    }
  }

  const bool IsEmpty() const {
    return m_X == 0 && m_Y == 0 && m_Width == 0 && m_Height == 0;
  }

private:
  T m_X = 0;
  T m_Y = 0;
  T m_Width = 0;
  T m_Height = 0;
};
} // namespace Nexus::Graphics