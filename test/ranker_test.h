/**
 * @file ranker_test.h
 * @author Sean Massung
 */

#ifndef _META_RANKER_TEST_H_
#define _META_RANKER_TEST_H_

#include "unit_test.h"
#include "inverted_index_test.h"
#include "index/ranker/all.h"

namespace meta
{
namespace testing
{

namespace
{
template <class Ranker, class Index>
void test_rank(Ranker& r, Index& idx)
{
    for (size_t i = 0; i < idx.num_docs(); ++i)
    {
        auto d_id = idx.docs()[i];
        corpus::document query{idx.doc_path(d_id), doc_id{i}};

        auto ranking = r.score(idx, query);
        ASSERT(ranking.size() == 10); // default is 10 docs

        // since we're searching for a document already in the index, the same
        // document should be ranked first, but there are a few duplicate
        // documents......
        if (ranking[0].first != i)
        {
            ASSERT(ranking[1].first == i);
            ASSERT(ranking[0].second == ranking[1].second);
        }
    }
}
}

void ranker_tests()
{
    create_config("file");
    system("/usr/bin/rm -rf ceeaus-inv");
    auto idx = index::make_index<index::inverted_index, caching::splay_cache>(
        "test-config.toml", uint32_t{10000});

    /* TODO why does this not always work?
    testing::run_test("ranker-absolute-discount", 5, [&]()
    {
        index::absolute_discount r;
        test_rank(r, idx);
    });
    */

    testing::run_test("ranker-dirichlet-prior", 5, [&]()
    {
        index::dirichlet_prior r;
        test_rank(r, idx);
    });

    testing::run_test("ranker-jelinek-mercer", 5, [&]()
    {
        index::jelinek_mercer r;
        test_rank(r, idx);
    });

    testing::run_test("ranker-okapi-bm25", 5, [&]()
    {
        index::okapi_bm25 r;
        test_rank(r, idx);
    });

    testing::run_test("ranker-pivoted-length", 5, [&]()
    {
        index::pivoted_length r;
        test_rank(r, idx);
    });

    system("/usr/bin/rm -rf ceeaus-inv test-config.toml");
}
}
}

#endif