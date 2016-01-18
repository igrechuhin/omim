#include "search/v2/search_model.hpp"

#include "indexer/classificator.hpp"
#include "indexer/ftypes_matcher.hpp"

#include "base/macros.hpp"

using namespace ftypes;

namespace search
{
namespace v2
{
namespace
{
class OneLevelPOIChecker : public ftypes::BaseChecker
{
public:
  OneLevelPOIChecker() : ftypes::BaseChecker(1 /* level */)
  {
    Classificator const & c = classif();
    m_types.push_back(c.GetTypeByPath({"amenity"}));
    m_types.push_back(c.GetTypeByPath({"historic"}));
    m_types.push_back(c.GetTypeByPath({"office"}));
    m_types.push_back(c.GetTypeByPath({"railway"}));
    m_types.push_back(c.GetTypeByPath({"shop"}));
    m_types.push_back(c.GetTypeByPath({"sport"}));
    m_types.push_back(c.GetTypeByPath({"tourism"}));
  }
};

class TwoLevelsPOIChecker : public ftypes::BaseChecker
{
public:
  TwoLevelsPOIChecker() : ftypes::BaseChecker(2 /* level */)
  {
    Classificator const & c = classif();
    m_types.push_back(c.GetTypeByPath({"highway", "bus_stop"}));
  }
};

class IsPoiChecker
{
public:
  IsPoiChecker() {}

  static IsPoiChecker const & Instance()
  {
    static const IsPoiChecker inst;
    return inst;
  }

  bool operator()(FeatureType const & ft) const { return m_oneLevel(ft) || m_twoLevels(ft); }

private:
  OneLevelPOIChecker const m_oneLevel;
  TwoLevelsPOIChecker const m_twoLevels;
};
}  // namespace

// static
SearchModel const & SearchModel::Instance()
{
  static SearchModel model;
  return model;
}

SearchModel::SearchType SearchModel::GetSearchType(FeatureType const & feature) const
{
  static auto const & buildingChecker = IsBuildingChecker::Instance();
  static auto const & streetChecker = IsStreetChecker::Instance();
  static auto const & localityChecker = IsLocalityChecker::Instance();
  static auto const & poiChecker = IsPoiChecker::Instance();

  if (buildingChecker(feature))
    return SEARCH_TYPE_BUILDING;

  if (streetChecker(feature))
    return SEARCH_TYPE_STREET;

  if (localityChecker(feature))
  {
    Type type = localityChecker.GetType(feature);
    switch (type)
    {
    case NONE:
    case STATE:
      return SEARCH_TYPE_COUNT;
    case COUNTRY:
      return SEARCH_TYPE_COUNTRY;
    case CITY:
    case TOWN:
    case VILLAGE:
      return SEARCH_TYPE_CITY;
    case LOCALITY_COUNT:
      return SEARCH_TYPE_COUNT;
    }
  }

  if (poiChecker(feature))
    return SEARCH_TYPE_POI;

  return SEARCH_TYPE_COUNT;
}

string DebugPrint(SearchModel::SearchType type)
{
  switch (type)
  {
  case SearchModel::SEARCH_TYPE_POI:
    return "POI";
  case SearchModel::SEARCH_TYPE_BUILDING:
    return "BUILDING";
  case SearchModel::SEARCH_TYPE_STREET:
    return "STREET";
  case SearchModel::SEARCH_TYPE_CITY:
    return "CITY";
  case SearchModel::SEARCH_TYPE_COUNTRY:
    return "COUNTRY";
  case SearchModel::SEARCH_TYPE_COUNT:
    return "COUNT";
  }
  ASSERT(false, ("Unknown search type:", static_cast<int>(type)));
  return string();
}
}  // namespace v2
}  // namespace search
