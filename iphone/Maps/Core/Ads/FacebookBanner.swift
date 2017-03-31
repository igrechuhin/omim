import FBAudienceNetwork

// MARK: FacebookBanner
final class FacebookBanner: FBNativeAd, Banner {
  fileprivate var success: Banner.Success!
  fileprivate var failure: Banner.Failure!

  func reload(success: @escaping Banner.Success, failure: @escaping Banner.Failure) {
    self.success = success
    self.failure = failure

    load()
    requestDate = Date()
  }

  var isPossibleToReload: Bool {
    if let date = requestDate {
      return Date().timeIntervalSince(date) > Limits.minTimeSinceLastRequest
    }
    return true
  }

  var isNeedToRetain: Bool = true
  var type: BannerType { return .facebook(bannerID) }
  var mwmType: MWMBannerType { return type.mwmType }
  var bannerID: String! { return placementID }
  var isBannerOnScreen = false {
    didSet {
      if isBannerOnScreen {
        startCountTimeOnScreen()
      } else {
        stopCountTimeOnScreen()
      }
    }
  }

  // MARK: Helpers
  private var requestDate: Date?
  private var remainingTime = Limits.minTimeOnScreen
  private var loadBannerDate: Date?

  private enum Limits {
    static let minTimeOnScreen: TimeInterval = 3
    static let minTimeSinceLastRequest: TimeInterval = 30
  }

  private func startCountTimeOnScreen() {
    if loadBannerDate == nil {
      loadBannerDate = Date()
    }

    if (remainingTime > 0) {
      perform(#selector(setEnoughTimeOnScreen), with: nil, afterDelay: remainingTime)
    }
  }

  private func stopCountTimeOnScreen() {
    guard let date = loadBannerDate else {
      assert(false)
      return
    }

    let timePassed = Date().timeIntervalSince(date)
    if (timePassed < Limits.minTimeOnScreen) {
      remainingTime = Limits.minTimeOnScreen - timePassed
      NSObject.cancelPreviousPerformRequests(withTarget: self)
    } else {
      remainingTime = 0
    }
  }

  @objc private func setEnoughTimeOnScreen() {
    isNeedToRetain = false
  }

  init(bannerID: String) {
    super.init(placementID: bannerID)
    mediaCachePolicy = .all
    delegate = self
    let center = NotificationCenter.default
    center.addObserver(self,
                       selector: #selector(enterForeground),
                       name: .UIApplicationWillEnterForeground,
                       object: nil)
    center.addObserver(self,
                       selector: #selector(enterBackground),
                       name: .UIApplicationDidEnterBackground,
                       object: nil)
  }

  @objc private func enterForeground() {
    if isBannerOnScreen {
      startCountTimeOnScreen()
    }
  }

  @objc private func enterBackground() {
    if (isBannerOnScreen) {
      stopCountTimeOnScreen()
    }
  }

  deinit {
    NotificationCenter.default.removeObserver(self)
  }
}

// MARK: FacebookBanner: FBNativeAdDelegate
extension FacebookBanner: FBNativeAdDelegate {

  func nativeAdDidLoad(_ nativeAd: FBNativeAd) {
    success(self)
  }

  func nativeAd(_ nativeAd: FBNativeAd, didFailWithError error: Error) {

    // https://developers.facebook.com/docs/audience-network/testing
    var params: [String: Any] = [kStatBanner : nativeAd.placementID, kStatProvider : kStatFacebook]

    let e = error as NSError
    let event: String
    if e.code == 1001 {
      event = kStatPlacePageBannerBlank
    } else {
      event = kStatPlacePageBannerError
      params[kStatErrorCode] = e.code
    }
    
    failure(self.type, event, params, e)
  }
}

