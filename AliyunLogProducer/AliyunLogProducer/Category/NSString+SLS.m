//
//  NSString+SLS.m
//  AliyunLogProducer
//
//  Created by gordon on 2022/8/11.
//

#import "NSString+SLS.h"

@implementation NSString (SLS)
- (NSString *) base64Encode {
    return [[self dataUsingEncoding:NSUTF8StringEncoding] base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength];
}

- (NSString *) base64Decode {
    return [[NSString alloc] initWithData:[[NSData alloc] initWithBase64EncodedString:self
                                                                              options:NSDataBase64DecodingIgnoreUnknownCharacters
                                          ]
                                 encoding:NSUTF8StringEncoding
    ];
}

- (NSDictionary *) toDictionary {
    NSData *data = [self dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;
    NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:data
                                    options:kNilOptions
                                      error:&error
    ];
    if (error) {
        NSLog(@"NSString to NSDictionary error. %@", error);
        return [NSDictionary dictionary];
    }
    
    return dict;
}
@end