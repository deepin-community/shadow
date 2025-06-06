#include <sys/types.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef SUBID_RANGE_DEFINED
#define SUBID_RANGE_DEFINED 1
#define SUBID_ABI_VERSION 5.0.0
#define SUBID_ABI_MAJOR 5
#define SUBID_ABI_MINOR 0
#define SUBID_ABI_MICRO 0

/* subid_range is just a starting point and size of a range */
struct subid_range {
	unsigned long start;
	unsigned long count;
};

/* subordinage_range is a subid_range plus an owner, representing
 * a range in /etc/subuid or /etc/subgid */
struct subordinate_range {
	const char *owner;
	unsigned long start;
	unsigned long count;
};

enum subid_type {
	ID_TYPE_UID = 1,
	ID_TYPE_GID = 2
};

enum subid_status {
	SUBID_STATUS_SUCCESS = 0,
	SUBID_STATUS_UNKNOWN_USER = 1,
	SUBID_STATUS_ERROR_CONN = 2,
	SUBID_STATUS_ERROR = 3,
};

#ifdef __cplusplus
extern "C" {
#endif

/*
 * subid_init: initialize libsubid
 *
 * @progname: Name to display as program.  If NULL, then "(libsubid)" will be
 *            shown in error messages.
 * @logfd:    Open file pointer to pass error messages to.  If NULL, then
 *            /dev/null will be opened and messages will be sent there.  The
 *            default if libsubid_init() is not called is stderr (2).
 *
 * This function does not need to be called.  If not called, then the defaults
 * will be used.
 *
 * Returns false if an error occurred.
 */
bool subid_init(const char *progname, FILE *logfd);

/*
 * subid_free: free memory allocated in any subid_* function
 *
 * @ptr: Pointer to a memory block to release.
 *
 * Some functions like @subid_get_uid_ranges allocate memory internally. As
 * soon as a result is no longer needed, it should be freed with this routine.
 * Initially, default function `free()` was used. Thus for backward
 * compatibility this function falls back to `free()` if a plugin does not
 * explicitly specify routine to free allocated memory.
 */
void subid_free(void *ptr);

/*
 * subid_get_uid_ranges: return a list of UID ranges for a user
 *
 * @owner: username being queried
 * @ranges: a pointer to an array of subid_range structs in which the result
 *          will be returned.
 *
 * The caller must free(ranges) when done.
 *
 * returns: number of ranges found, ir < 0 on error.
 */
int subid_get_uid_ranges(const char *owner, struct subid_range **ranges);

/*
 * subid_get_gid_ranges: return a list of GID ranges for a user
 *
 * @owner: username being queried
 * @ranges: a pointer to an array of subid_range structs in which the result
 *          will be returned.
 *
 * The caller must free(ranges) when done.
 *
 * returns: number of ranges found, ir < 0 on error.
 */
int subid_get_gid_ranges(const char *owner, struct subid_range **ranges);

/*
 * subid_get_uid_owners: return a list of uids to which the given uid has been
 *                    delegated.
 *
 * @uid: The subuid being queried
 * @owners: a pointer to an array of uids into which the results are placed.
 *          The returned array must be freed by the caller.
 *
 * Returns the number of uids returned, or < 0 on error.
 */
int subid_get_uid_owners(uid_t uid, uid_t **owner);

/*
 * subid_get_gid_owners: return a list of uids to which the given gid has been
 *                    delegated.
 *
 * @uid: The subgid being queried
 * @owners: a pointer to an array of uids into which the results are placed.
 *          The returned array must be freed by the caller.
 *
 * Returns the number of uids returned, or < 0 on error.
 */
int subid_get_gid_owners(gid_t gid, uid_t **owner);

/*
 * subid_grant_uid_range: assign a subuid range to a user
 *
 * @range: pointer to a struct subordinate_range detailing the UID range
 *         to allocate.  ->owner must be the username, and ->count must be
 *         filled in.  ->start is ignored, and will contain the start
 *         of the newly allocated range, upon success.
 *
 * Returns true if the delegation succeeded, false otherwise.  If true,
 * then the range from (range->start, range->start + range->count) will
 * be delegated to range->owner.
 */
bool subid_grant_uid_range(struct subordinate_range *range, bool reuse);

/*
 * subid_grant_gid_range: assign a subgid range to a user
 *
 * @range: pointer to a struct subordinate_range detailing the GID range
 *         to allocate.  ->owner must be the username, and ->count must be
 *         filled in.  ->start is ignored, and will contain the start
 *         of the newly allocated range, upon success.
 *
 * Returns true if the delegation succeeded, false otherwise.  If true,
 * then the range from (range->start, range->start + range->count) will
 * be delegated to range->owner.
 */
bool subid_grant_gid_range(struct subordinate_range *range, bool reuse);

/*
 * subid_ungrant_uid_range: remove a subuid allocation.
 *
 * @range: pointer to a struct subordinate_range detailing the UID allocation
 *         to remove.
 *
 * Returns true if successful, false if it failed, for instance if the
 * delegation did not exist.
 */
bool subid_ungrant_uid_range(struct subordinate_range *range);

/*
 * subid_ungrant_gid_range: remove a subgid allocation.
 *
 * @range: pointer to a struct subordinate_range detailing the GID allocation
 *         to remove.
 *
 * Returns true if successful, false if it failed, for instance if the
 * delegation did not exist.
 */
bool subid_ungrant_gid_range(struct subordinate_range *range);

#ifdef __cplusplus
}
#endif

#define SUBID_NFIELDS 3
#endif
