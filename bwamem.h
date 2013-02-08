#ifndef BWAMEM_H_
#define BWAMEM_H_

#include "bwt.h"
#include "bntseq.h"
#include "bseq.h"
#include "kvec.h"

struct __smem_i;
typedef struct __smem_i smem_i;

typedef struct {
	int64_t rbeg;
	int32_t qbeg, len;
} mem_seed_t;

typedef struct {
	int a, b, q, r, w;
	int split_width;
	int min_seed_len, max_occ, max_chain_gap;
	int n_threads, chunk_size;
	int pe_dir, is_pe;
	float mask_level, chain_drop_ratio;
	int8_t mat[25]; // scoring matrix; mat[0] == 0 if unset
} mem_opt_t;

typedef struct {
	int n, m;
	int64_t pos;
	mem_seed_t *seeds;
} mem_chain_t;

typedef struct {
	int64_t rb, re;
	int score, qb, qe, seedcov, sub, csub; // sub: suboptimal score; csub: suboptimal inside the chain
	double sub_n;
} mem_alnreg_t;

typedef kvec_t(mem_chain_t)  mem_chain_v;
typedef kvec_t(mem_alnreg_t) mem_alnreg_v;

#ifdef __cplusplus
extern "C" {
#endif

smem_i *smem_itr_init(const bwt_t *bwt);
void smem_itr_destroy(smem_i *itr);
void smem_set_query(smem_i *itr, int len, const uint8_t *query);
const bwtintv_v *smem_next(smem_i *itr, int split_len, int split_width);

mem_opt_t *mem_opt_init(void);
void mem_fill_scmat(int a, int b, int8_t mat[25]);

mem_chain_v mem_chain(const mem_opt_t *opt, const bwt_t *bwt, int len, const uint8_t *seq);
int mem_chain_flt(const mem_opt_t *opt, int n_chn, mem_chain_t *chains);
void mem_chain2aln(const mem_opt_t *opt, int64_t l_pac, const uint8_t *pac, int l_query, const uint8_t *query, const mem_chain_t *c, mem_alnreg_t *a);
uint32_t *mem_gen_cigar(const mem_opt_t *opt, int64_t l_pac, const uint8_t *pac, int l_query, uint8_t *query, int64_t rb, int64_t re, int *score, int *n_cigar);

int mem_process_seqs(const mem_opt_t *opt, const bwt_t *bwt, const bntseq_t *bns, const uint8_t *pac, int n, bseq1_t *seqs);

#ifdef __cplusplus
}
#endif

#endif
