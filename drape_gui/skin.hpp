#pragma once

#include "drape/drape_global.hpp"
#include "geometry/point2d.hpp"
#include "coding/reader.hpp"

#include "std/map.hpp"

namespace gui
{

enum EWidget
{
  WIDGET_RULER = 0x1,
  WIDGET_COMPASS = 0x2,
  WIDGET_COPYRIGHT = 0x4,
  WIDGET_SCALE_LABLE = 0x8,
  /// WIDGET_COUNTRY_STATUS - controlled by rendering kernel. Don't use it in platform code
  WIDGET_COUNTRY_STATUS = 0x8000
};

struct Position
{
  Position() : m_pixelPivot(m2::PointF::Zero()), m_anchor(dp::Center) {}
  explicit Position(dp::Anchor anchor)
    : m_pixelPivot(m2::PointF::Zero())
    , m_anchor(anchor) {}

  Position(m2::PointF const & pt, dp::Anchor anchor)
    : m_pixelPivot(pt)
    , m_anchor(anchor) {}

  m2::PointF m_pixelPivot;
  dp::Anchor m_anchor;
};

/// TWidgetsInitInfo - static information about gui widgets (geometry align)
using TWidgetsInitInfo = map<EWidget, gui::Position>;
/// TWidgetsLayoutInfo - dynamic info. Pivot point of each widget in pixel coordinates
using TWidgetsLayoutInfo = map<EWidget, m2::PointD>;
/// TWidgetsSizeInfo - info about widget pixel sizes
using TWidgetsSizeInfo = TWidgetsLayoutInfo;

class PositionResolver
{
public:
  Position Resolve(int w, int h, double vs) const;
  void AddAnchor(dp::Anchor anchor);
  void AddRelative(dp::Anchor anchor);
  void SetOffsetX(float x);
  void SetOffsetY(float y);

private:
  dp::Anchor m_elementAnchor = dp::Center;
  dp::Anchor m_resolveAnchor = dp::Center;
  m2::PointF m_offset = m2::PointF::Zero();
};

class Skin
{
public:
  Skin(ReaderPtr<Reader> const & reader, float visualScale);

  /// @param name - must be single flag, not combination
  Position ResolvePosition(EWidget name);
  void Resize(int w, int h);
  int GetWidth() const { return m_displayWidth; }
  int GetHeight() const { return m_displayHeight; }

  template <typename ToDo>
  void ForEach(ToDo todo)
  {
    for (auto node : m_resolvers)
      todo(node.first, ResolvePosition(node.first));
  }

private:
  /// TResolversPair.first - Portrait (when weight < height)
  /// TResolversPair.second - Landscape (when weight >= height)
  typedef pair<PositionResolver, PositionResolver> TResolversPair;
  map<EWidget, TResolversPair> m_resolvers;

  int m_displayWidth;
  int m_displayHeight;
  float m_visualScale;
};

ReaderPtr<Reader> ResolveGuiSkinFile(string const & deviceType);

}
