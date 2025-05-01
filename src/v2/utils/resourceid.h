#include <atomic>
#include <set>
#include <type_traits>

#ifndef RESOURCEID_H
#define RESOURCEID_H

namespace pdflib
{

    class resource_id_set {
    public:
        uint64_t id;

        resource_id_set() {
            id = allocate();
        }

        void add(const uint64_t id) {
            member_ids.insert(id);
        }

        // Generic add for any iterable container of uint64_t
        template <typename Iterable>
        void add(const Iterable& ids) {
            using std::begin;
            using std::end;
            for (auto it = begin(ids); it != end(ids); ++it) {
                member_ids.insert(*it);
            }
        }

        void add(const resource_id_set& other) {
            add(other.id);
            add(other.member_ids);
        }

        const std::set<uint64_t>& get_member_ids() const { return member_ids; }

        // Generic set_member_ids for any iterable container of uint64_t
        template <typename Iterable>
        void set_member_ids(const Iterable& ids) {
            member_ids.clear();
            using std::begin;
            using std::end;
            for (auto it = begin(ids); it != end(ids); ++it) {
                member_ids.insert(*it);
            }
        }

        resource_id_set merge(const resource_id_set& other) const {
            // Merge here is a bit weird, we return a new
            // id set with a new main ID.  The reason is that
            // resources get merged, serialized, deserialzed, etc 
            // at different times, so there isn't a single
            // lineage for any resource, so if we create a new ID each time
            // this avoids the same ID being used for different grouping.
            // The underlying ID gets added to the members set, so we can still trace back.
            resource_id_set result;
            result.add(id);
            result.add(member_ids);
            result.add(other);
            return result;
        }

    private:
        static uint64_t allocate() {
            return counter.fetch_add(1, std::memory_order_relaxed);
        }

        static std::atomic<uint64_t> counter;
        std::set<uint64_t> member_ids;
    };

    // Define the static member
    std::atomic<uint64_t> resource_id_set::counter{0};
}

#endif