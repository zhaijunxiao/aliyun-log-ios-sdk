//
//  SLSNetworkDiagnosis.m
//  AliyunLogProducer
//
//  Created by gordon on 2022/8/10.
//

#import "SLSNetworkDiagnosis.h"

@interface SLSNetworkDiagnosis ()
@property(nonatomic, strong) SLSNetworkDiagnosisFeature *feature;
@end

@implementation SLSNetworkDiagnosis

+ (instancetype) sharedInstance {
    static SLSNetworkDiagnosis * ins = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        ins = [[SLSNetworkDiagnosis alloc] init];
    });
    return ins;
}

- (void) setNetworkDiagnosisFeature: (SLSNetworkDiagnosisFeature *) feature {
    _feature = feature;
}

- (void) disableExNetworkInfo {
    if (!_feature) {
        return;
    }
    
    [_feature disableExNetworkInfo];
}

- (void) setMultiplePortsDetect: (BOOL) enable {
    if (!_feature) {
        return;
    }
    
    [_feature setMultiplePortsDetect:enable];
}

- (void) setPolicyDomain: (NSString *) policyDomain {
    if (!_feature) {
        return;
    }
    
    [_feature setPolicyDomain:policyDomain];
}

- (void) registerCallback: (nullable Callback) callback {
    if (!_feature) {
        return;
    }
    
    [_feature registerCallback:callback];
}

- (void)dns:(nonnull NSString *)domain {
    if (!_feature) {
        return;
    }
    
    [_feature dns:domain];
}

- (void)dns:(nonnull NSString *)domain callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature dns:domain callback:callback];
}

- (void)dns:(nonnull NSString *)nameServer domain:(nonnull NSString *)domain callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature dns:nameServer domain:domain callback:callback];
}

- (void)dns:(nonnull NSString *)nameServer domain:(nonnull NSString *)domain type:(nonnull NSString *)type callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature dns:nameServer domain:domain type:type callback:callback];
}

- (void)dns:(nonnull NSString *)nameServer domain:(nonnull NSString *)domain type:(nonnull NSString *)type timeout:(int)timeout callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature dns:nameServer domain:domain type:type timeout:timeout callback:callback];
}

- (void)http:(nonnull NSString *)url {
    if (!_feature) {
        return;
    }
    
    [_feature http:url];
}

- (void)http:(nonnull NSString *)url callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature http:url callback:callback];
}

- (void)mtr:(nonnull NSString *)domain {
    if (!_feature) {
        return;
    }
    
    [_feature mtr:domain];
}

- (void)mtr:(nonnull NSString *)domain callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature mtr:domain callback:callback];
}

- (void)mtr:(nonnull NSString *)domain maxTTL:(int)maxTTL callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature mtr:domain maxTTL:maxTTL callback:callback];
}

- (void)mtr:(nonnull NSString *)domain maxTTL:(int)maxTTL maxPaths:(int)maxPaths callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
 
    [_feature mtr:domain maxTTL:maxTTL maxPaths:maxPaths callback:callback];
}

- (void)mtr:(nonnull NSString *)domain maxTTL:(int)maxTTL maxPaths:(int)maxPaths maxTimes:(int)maxTimes callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature mtr:domain maxTTL:maxTTL maxPaths:maxPaths maxTimes:maxTimes callback:callback];
}

- (void)mtr:(nonnull NSString *)domain maxTTL:(int)maxTTL maxPaths:(int)maxPaths maxTimes:(int)maxTimes timeout:(int)timeout callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature mtr:domain maxTTL:maxTTL maxPaths:maxPaths maxTimes:maxTimes timeout:timeout callback:callback];
}

- (void)ping:(nonnull NSString *)domain {
    if (!_feature) {
        return;
    }
    
    [_feature ping:domain];
}

- (void)ping:(nonnull NSString *)domain callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature ping:domain callback:callback];
}

- (void)ping:(nonnull NSString *)domain maxTimes:(int)maxTimes timeout:(int)timeout callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature ping:domain maxTimes:maxTimes timeout:timeout callback:callback];
}

- (void)ping:(nonnull NSString *)domain size:(int)size callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature ping:domain size:size callback:callback];
}

- (void)ping:(nonnull NSString *)domain size:(int)size maxTimes:(int)maxTimes timeout:(int)timeout callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature ping:domain size:size maxTimes:maxTimes timeout:timeout callback:callback];
}

- (void)tcpPing:(nonnull NSString *)domain port:(int)port {
    if (!_feature) {
        return;
    }
    
    [_feature tcpPing:domain port:port];
}

- (void)tcpPing:(nonnull NSString *)domain port:(int)port callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature tcpPing:domain port:port callback:callback];
}

- (void)tcpPing:(nonnull NSString *)domain port:(int)port maxTimes:(int)maxTimes callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature tcpPing:domain port:port maxTimes:maxTimes callback:callback];
}

- (void)tcpPing:(nonnull NSString *)domain port:(int)port maxTimes:(int)maxTimes timeout:(int)timeout callback:(nullable Callback)callback {
    if (!_feature) {
        return;
    }
    
    [_feature tcpPing:domain port:port maxTimes:maxTimes timeout:timeout callback:callback];
}

@end