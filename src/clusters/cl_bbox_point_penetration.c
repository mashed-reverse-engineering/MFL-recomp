// cluster-entries: bbox_point_penetration
/* Installed cluster bbox_point_penetration (tools/cluster_install.py from output/cluster_pilot_uc3/bbox_point_penetration.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"

/* Squared depth of the point `p` against the box `box` (sup corner first, inf corner second).
   Point inside the box on two axes: the depth along the third one, measured to the box face on the point's side of the origin and never negative (squared).
   Otherwise the squared distance from the box (the axes inside contribute nothing); with all three axes outside the y term is rounded to a float.
   The branch structure follows the original (its x87 stack decides the rounding).  An axis that is neither above nor below (inside, or NaN) can read an
   uninitialised slot in the original: the argument slot that holds `p`; reproduced through `slot` (only NaN coordinates reach it). */
double bbox_point_penetration(RwBBox *box, RwV3d *p)
{
  union { RwV3d *ptr; float value; } slot;
  double d, dx2, dz;
  volatile float dy2;       /* a stored 32 bit float in the original (fstp dword): the square is rounded to float before it is added */

  slot.ptr = p;
  if (p->x <= box->sup.x && box->inf.x <= p->x && p->y <= box->sup.y && box->inf.y <= p->y) {
    d = (p->z > 0.0f) ? p->z - box->sup.z : box->inf.z - p->z;
    if (d < 0.0) {
      d = 0.0;
    }
    return d * d;
  }
  if (p->x <= box->sup.x && box->inf.x <= p->x && p->z <= box->sup.z && box->inf.z <= p->z) {
    d = (p->y > 0.0f) ? p->y - box->sup.y : box->inf.y - p->y;
    if (d < 0.0) {
      d = 0.0;
    }
    return d * d;
  }
  if (p->y <= box->sup.y && box->inf.y <= p->y && p->z <= box->sup.z && box->inf.z <= p->z) {
    d = (p->x > 0.0f) ? p->x - box->sup.x : box->inf.x - p->x;
    if (d < 0.0) {
      d = 0.0;
    }
    return d * d;
  }
  if (p->x > box->sup.x) {
    d = p->x - box->sup.x;
  } else if (p->x < box->inf.x) {
    d = box->inf.x - p->x;
  } else {
    /* x inside: the y and z terms only */
    if (p->y >= box->sup.y) {
      d = p->y - box->sup.y;
    } else if (p->y <= box->inf.y) {
      d = box->inf.y - p->y;
    } else {
      d = slot.value;
    }
    d = d * d;
    if (p->z >= box->sup.z) {
      dz = p->z - box->sup.z;
    } else if (p->z <= box->inf.z) {
      dz = box->inf.z - p->z;
    } else {
      dz = slot.value;
    }
    return dz * dz + d;
  }
  dx2 = d * d;
  if (p->y > box->sup.y) {
    d = p->y - box->sup.y;
  } else if (p->y < box->inf.y) {
    d = box->inf.y - p->y;
  } else {
    /* y inside, x outside: the x and z terms */
    if (p->x >= box->sup.x) {
      d = p->x - box->sup.x;
    } else if (p->x <= box->inf.x) {
      d = box->inf.x - p->x;
    } else {
      d = dx2;
    }
    d = d * d;
    if (p->z >= box->sup.z) {
      dz = p->z - box->sup.z;
    } else if (p->z <= box->inf.z) {
      dz = box->inf.z - p->z;
    } else {
      dz = slot.value;
    }
    return dz * dz + d;
  }
  dy2 = d * d;
  if (p->z > box->sup.z) {
    dz = p->z - box->sup.z;
    return dx2 + (dz * dz + dy2);
  }
  if (p->z < box->inf.z) {
    dz = box->inf.z - p->z;
    return dx2 + (dz * dz + dy2);
  }
  /* z inside: the x and y terms */
  if (p->x >= box->sup.x) {
    d = p->x - box->sup.x;
  } else if (p->x <= box->inf.x) {
    d = box->inf.x - p->x;
  } else {
    d = dx2;
  }
  dx2 = d * d;
  if (p->y >= box->sup.y) {
    d = p->y - box->sup.y;
    return d * d + dx2;
  }
  if (p->y <= box->inf.y) {
    d = box->inf.y - p->y;
    return d * d + dx2;
  }
  return slot.value * slot.value + dx2;
}

