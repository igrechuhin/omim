//
//  MWMNetworkPolicyHelper.m
//  Maps
//
//  Created by Ilya Grechuhin on 10.11.2017.
//  Copyright © 2017 MapsWithMe. All rights reserved.
//

#import "MWMNetworkPolicyHelper.h"
#import "MWMNetworkPolicy.h"

@implementation MWMNetworkPolicyHelper

+ (BOOL)canUseNetwork { return network_policy::CanUseNetwork(); }

@end
