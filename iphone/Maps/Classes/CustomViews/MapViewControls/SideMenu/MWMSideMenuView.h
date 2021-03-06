#import "MWMSideMenuDelegate.h"
#import "MWMSideMenuDownloadBadge.h"

@interface MWMSideMenuView : SolidTouchView <MWMSideMenuDownloadBadgeOwner>

@property (weak, nonatomic, readonly) IBOutlet UIView * dimBackground;
@property (weak, nonatomic) id<MWMSideMenuInformationDisplayProtocol> delegate;
@property (weak, nonatomic) MWMSideMenuDownloadBadge * downloadBadge;

- (instancetype)initWithFrame:(CGRect)frame __attribute__((unavailable("initWithFrame is not available")));
- (instancetype)init __attribute__((unavailable("init is not available")));

- (void)setup;

@end
