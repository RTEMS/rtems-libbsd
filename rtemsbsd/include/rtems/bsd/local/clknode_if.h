/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/extres/clk/clknode_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _clknode_if_h_
#define _clknode_if_h_


struct clknode;

/** @brief Unique descriptor for the CLKNODE_INIT() method */
extern struct kobjop_desc clknode_init_desc;
/** @brief A function implementing the CLKNODE_INIT() method */
typedef int clknode_init_t(struct clknode *clk, device_t dev);

static __inline int CLKNODE_INIT(struct clknode *clk, device_t dev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)clk)->ops,clknode_init);
	rc = ((clknode_init_t *) _m)(clk, dev);
	return (rc);
}

/** @brief Unique descriptor for the CLKNODE_RECALC_FREQ() method */
extern struct kobjop_desc clknode_recalc_freq_desc;
/** @brief A function implementing the CLKNODE_RECALC_FREQ() method */
typedef int clknode_recalc_freq_t(struct clknode *clk, uint64_t *freq);

static __inline int CLKNODE_RECALC_FREQ(struct clknode *clk, uint64_t *freq)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)clk)->ops,clknode_recalc_freq);
	rc = ((clknode_recalc_freq_t *) _m)(clk, freq);
	return (rc);
}

/** @brief Unique descriptor for the CLKNODE_SET_FREQ() method */
extern struct kobjop_desc clknode_set_freq_desc;
/** @brief A function implementing the CLKNODE_SET_FREQ() method */
typedef int clknode_set_freq_t(struct clknode *clk, uint64_t fin,
                               uint64_t *fout, int flags, int *done);

static __inline int CLKNODE_SET_FREQ(struct clknode *clk, uint64_t fin,
                                     uint64_t *fout, int flags, int *done)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)clk)->ops,clknode_set_freq);
	rc = ((clknode_set_freq_t *) _m)(clk, fin, fout, flags, done);
	return (rc);
}

/** @brief Unique descriptor for the CLKNODE_SET_GATE() method */
extern struct kobjop_desc clknode_set_gate_desc;
/** @brief A function implementing the CLKNODE_SET_GATE() method */
typedef int clknode_set_gate_t(struct clknode *clk, bool enable);

static __inline int CLKNODE_SET_GATE(struct clknode *clk, bool enable)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)clk)->ops,clknode_set_gate);
	rc = ((clknode_set_gate_t *) _m)(clk, enable);
	return (rc);
}

/** @brief Unique descriptor for the CLKNODE_SET_MUX() method */
extern struct kobjop_desc clknode_set_mux_desc;
/** @brief A function implementing the CLKNODE_SET_MUX() method */
typedef int clknode_set_mux_t(struct clknode *clk, int idx);

static __inline int CLKNODE_SET_MUX(struct clknode *clk, int idx)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)clk)->ops,clknode_set_mux);
	rc = ((clknode_set_mux_t *) _m)(clk, idx);
	return (rc);
}

#endif /* _clknode_if_h_ */
