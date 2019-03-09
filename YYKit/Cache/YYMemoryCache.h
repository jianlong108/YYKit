//
//  YYMemoryCache.h
//  YYKit <https://github.com/ibireme/YYKit>
//
//  Created by ibireme on 15/2/7.
//  Copyright (c) 2015 ibireme.
//
//  This source code is licensed under the MIT-style license found in the
//  LICENSE file in the root directory of this source tree.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 YYMemoryCache 是一个高速的内存缓存，用于存储键值对。它与 NSDictionary 相反，Key 被保留并且不复制。API 和性能类似于 NSCache，所有方法都是线程安全的。
 YYMemoryCache 对象与 NSCache 的不同之处在于：
 YYMemoryCache 使用 LRU(least-recently-used) 算法来驱逐对象；NSCache 的驱逐方式是非确定性的。
 YYMemoryCache 提供 age、cost、count 三种方式控制缓存；NSCache 的控制方式是不精确的。
 YYMemoryCache 可以配置为在收到内存警告或者 App 进入后台时自动逐出对象。
 
 YYMemoryCache is a fast in-memory cache that stores key-value pairs.
 In contrast to NSDictionary, keys are retained and not copied.
 The API and performance is similar to `NSCache`, all methods are thread-safe.
 
 YYMemoryCache objects differ from NSCache in a few ways:
 
 * It uses LRU (least-recently-used) to remove objects; NSCache's eviction method
   is non-deterministic.
 * It can be controlled by cost, count and age; NSCache's limits are imprecise.
 * It can be configured to automatically evict objects when receive memory 
   warning or app enter background.
 
 The time of `Access Methods` in YYMemoryCache is typically in constant time (O(1)).
 */
@interface YYMemoryCache : NSObject

#pragma mark - Attribute
///=============================================================================
/// @name Attribute
///=============================================================================

/** The name of the cache. Default is nil. */
@property (nullable, copy) NSString *name;

/** The number of objects in the cache (read-only) 缓存对象的总数量*/
@property (readonly) NSUInteger totalCount;

/** The total cost of objects in the cache (read-only). 缓存对象的总开销*/
@property (readonly) NSUInteger totalCost;


#pragma mark - Limit
///=============================================================================
/// @name Limit
///=============================================================================

/**
 The maximum number of objects the cache should hold.
 ￥￥缓存对象数量限制，默认无限制，超过限制则会在后台逐出一些对象以满足限制
 @discussion The default value is NSUIntegerMax, which means no limit.
 This is not a strict limit—if the cache goes over the limit, some objects in the
 cache could be evicted later in backgound thread.
 */
@property NSUInteger countLimit;

/**
 The maximum total cost that the cache can hold before it starts evicting objects.
 ￥￥ 缓存开销数量限制，默认无限制，超过限制则会在后台逐出一些对象以满足限制
 @discussion The default value is NSUIntegerMax, which means no limit.
 This is not a strict limit—if the cache goes over the limit, some objects in the
 cache could be evicted later in backgound thread.
 */
@property NSUInteger costLimit;

/**
 The maximum expiry time of objects in cache.
 ￥￥缓存时间限制，默认无限制，超过限制则会在后台逐出一些对象以满足限制
 @discussion The default value is DBL_MAX, which means no limit.
 This is not a strict limit—if an object goes over the limit, the object could 
 be evicted later in backgound thread.
 */
@property NSTimeInterval ageLimit;

/**
 The auto trim check time interval in seconds. Default is 5.0.
 ￥￥缓存自动清理时间间隔，默认 5s
 @discussion The cache holds an internal timer to check whether the cache reaches 
 its limits, and if the limit is reached, it begins to evict objects.
 */
@property NSTimeInterval autoTrimInterval;

/**
 If `YES`, the cache will remove all objects when the app receives a memory warning.
 The default value is `YES`.
 ￥￥是否应该在收到内存警告时删除所有缓存内对象
 */
@property BOOL shouldRemoveAllObjectsOnMemoryWarning;

/**
 If `YES`, The cache will remove all objects when the app enter background.
 The default value is `YES`.
 ￥￥是否应该在 App 进入后台时删除所有缓存内对象
 */
@property BOOL shouldRemoveAllObjectsWhenEnteringBackground;

/**
 ￥￥内部注册了内存警告通知，当收到内存警告时，可以自定义操作
 A block to be executed when the app receives a memory warning.
 The default value is nil.
 */
@property (nullable, copy) void(^didReceiveMemoryWarningBlock)(YYMemoryCache *cache);

/**
 ￥￥内部注册了进入后台通知，当收到内存警告时，可以自定义操作
 A block to be executed when the app enter background.
 The default value is nil.
 */
@property (nullable, copy) void(^didEnterBackgroundBlock)(YYMemoryCache *cache);

/**
 If `YES`, the key-value pair will be released on main thread, otherwise on
 background thread. Default is NO.
 ￥￥是否在主线程释放对象，默认 NO，有些对象（例如 UIView/CALayer）应该在主线程释放
 @discussion You may set this value to `YES` if the key-value object contains
 the instance which should be released in main thread (such as UIView/CALayer).
 */
@property BOOL releaseOnMainThread;

/**
 If `YES`, the key-value pair will be released asynchronously to avoid blocking 
 the access methods, otherwise it will be released in the access method  
 (such as removeObjectForKey:). Default is YES.
 ￥￥是否异步释放对象，默认 YES
 */
@property BOOL releaseAsynchronously;


#pragma mark - Access Methods
///=============================================================================
/// @name Access Methods
///=============================================================================

/**
 Returns a Boolean value that indicates whether a given key is in cache.
 
 @param key An object identifying the value. If nil, just return `NO`.
 @return Whether the key is in cache.
 */
- (BOOL)containsObjectForKey:(id)key;

/**
 Returns the value associated with a given key.
 
 @param key An object identifying the value. If nil, just return nil.
 @return The value associated with key, or nil if no value is associated with key.
 */
- (nullable id)objectForKey:(id)key;

/**
 Sets the value of the specified key in the cache (0 cost).
 
 @param object The object to be stored in the cache. If nil, it calls `removeObjectForKey:`.
 @param key    The key with which to associate the value. If nil, this method has no effect.
 @discussion Unlike an NSMutableDictionary object, a cache does not copy the key 
 objects that are put into it.
 */
- (void)setObject:(nullable id)object forKey:(id)key;

/**
 Sets the value of the specified key in the cache, and associates the key-value 
 pair with the specified cost.
 
 @param object The object to store in the cache. If nil, it calls `removeObjectForKey`.
 @param key    The key with which to associate the value. If nil, this method has no effect.
 @param cost   The cost with which to associate the key-value pair.
 @discussion Unlike an NSMutableDictionary object, a cache does not copy the key
 objects that are put into it.
 */
- (void)setObject:(nullable id)object forKey:(id)key withCost:(NSUInteger)cost;

/**
 Removes the value of the specified key in the cache.
 
 @param key The key identifying the value to be removed. If nil, this method has no effect.
 */
- (void)removeObjectForKey:(id)key;

/**
 Empties the cache immediately.
 */
- (void)removeAllObjects;


#pragma mark - Trim
///=============================================================================
/// @name Trim 修剪、整理
///=============================================================================

/**
 ￥￥用 LRU 算法删除对象，直到 totalCount <= count
 Removes objects from the cache with LRU, until the `totalCount` is below or equal to
 the specified value.
 @param count  The total count allowed to remain after the cache has been trimmed.
 */
- (void)trimToCount:(NSUInteger)count;

/**
 ￥￥ 用 LRU 算法删除对象，直到 totalCost <= cost
 Removes objects from the cache with LRU, until the `totalCost` is or equal to
 the specified value.
 @param cost The total cost allowed to remain after the cache has been trimmed.
 */
- (void)trimToCost:(NSUInteger)cost;

/**
 ￥￥ 用 LRU 算法删除对象，直到所有到期对象全部被删除
 Removes objects from the cache with LRU, until all expiry objects removed by the
 specified value.
 @param age  The maximum age (in seconds) of objects.
 */
- (void)trimToAge:(NSTimeInterval)age;

@end

NS_ASSUME_NONNULL_END
