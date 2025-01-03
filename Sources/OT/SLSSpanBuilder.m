//
//  SLSSpanBuiler.m
//  AliyunLogProducer
//
//  Created by gordon on 2022/4/27.
//

#import "SLSSpanBuilder.h"
#import "SLSRecordableSpan.h"
#import "SLSIdGenerator.h"
#import "SLSTimeUtils.h"
#import "SLSContextManager.h"

@interface SLSSpanBuilder ()
@property(nonatomic, strong) NSString *name;
@property(nonatomic, strong) id<SLSSpanProcessorProtocol> spanProcessor;
@property(nonatomic, strong) id<SLSSpanProviderProtocol> spanProvider;
@property(nonatomic, strong, readonly) SLSSpan *parent;
@property(atomic, assign, readonly) BOOL active;
@property(nonnull, strong, readonly) SLSKind kind;
@property(nonatomic, strong) NSMutableArray<SLSAttribute*> *attributes;
@property(nonatomic, strong, readonly) SLSResource *resource;
@property(nonatomic, assign, readonly) long start;
@property(nonatomic, strong, readonly) NSString *service;
@property(atomic, assign, readonly) BOOL global;
@end

@implementation SLSSpanBuilder

+ (SLSSpanBuilder *) builder {
    return [[SLSSpanBuilder alloc] init];
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        _attributes = [NSMutableArray<SLSAttribute*> array];
        _start = 0L;
        _kind = SLSCLIENT;
        _global = YES;
    }
    return self;
}
- (SLSSpanBuilder *) initWithName: (NSString *)name provider: (id<SLSSpanProviderProtocol>) provider processor: (id<SLSSpanProcessorProtocol>) processor {
    self = [self init];
    if (self) {
        _name = name;
        _spanProcessor = processor;
        _spanProvider = provider;
    }
    return self;
}

- (SLSSpanBuilder *) setParent: (SLSSpan *)parent {
    _parent = parent;
    return self;
}

- (SLSSpanBuilder *) setActive: (BOOL) active {
    _active = active;
    return self;
}
- (SLSSpanBuilder *) setKind: (SLSKind) kind {
    _kind = kind;
    return self;
}

- (SLSSpanBuilder *) addAttribute: (SLSAttribute *) attribute, ... NS_REQUIRES_NIL_TERMINATION {
    [_attributes addObject:attribute];
    
    va_list args;
    SLSAttribute *arg;
    va_start(args, attribute);
    while ((arg = va_arg(args, SLSAttribute*))) {
        [_attributes addObject:arg];
    }
    va_end(args);
    
    return self;
}

- (SLSSpanBuilder *) addAttributes: (NSArray<SLSAttribute *> *) attributes {
    [_attributes addObjectsFromArray:attributes];
    return self;
}

- (SLSSpanBuilder *) setStart: (long) start {
    _start = start;
    return self;
}
- (SLSSpanBuilder *) addResource: (SLSResource *) resource {
    _resource = resource;
    return self;
}
- (SLSSpanBuilder *) setService: (NSString *)service {
    _service = service;
    return self;
}
- (SLSSpanBuilder *) setGlobal: (BOOL) global {
    _global = global;
    return self;
}
- (SLSSpan *) build {
    SLSRecordableSpan *span = [[SLSRecordableSpan alloc] initWithSpanProcessor:_spanProcessor];
    span.name = _name;
    span.service = _service;
    span.kind = _kind;
    span.spanID = SLSIdGenerator.generateSpanId;
    
    SLSSpan *parentSpan = nil;
    if (nil != _parent) {
        parentSpan = _parent;
    } else {
        parentSpan = [SLSContextManager activeSpan];
    }
    
    if (nil != parentSpan) {
        span.traceID = parentSpan.traceID;
        span.parentSpanID = parentSpan.spanID;
    } else {
        span.traceID = SLSIdGenerator.generateTraceId;
    }
    
    if (nil != _spanProvider) {
        [_attributes addObjectsFromArray:[_spanProvider provideAttribute]];
    }
    NSMutableDictionary<NSString *, NSString *> *dict = (NSMutableDictionary<NSString *, NSString *> *) span.attribute;
    for (SLSAttribute *attr in _attributes) {
        if (attr.key && attr.value) {
            [dict setObject:attr.value forKey:attr.key];
        }
    }
    
    SLSResource *r = [SLSResource resource];
    if (nil != _spanProvider) {
        [r merge:[_spanProvider provideResource]];
    }
    if (nil != _resource) {
        [r merge:_resource];
    }
    span.resource = r;
    [span setGlobal: _global];
    
    if (_start != 0L) {
        span.start = _start;
    } else {
        span.start = SLSTimeUtils.now;
    }
    
    if (_active) {
        [span setScope:[SLSContextManager makeCurrent:span]];
    }
    
    if (span.isGlobal) {
        [SLSContextManager setGlobalActiveSpan:span];
    }
    
    return span;
}

@end
