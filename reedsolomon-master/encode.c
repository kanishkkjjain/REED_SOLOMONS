#include <string.h>

#include "Headers/array.h"
#include "Headers/op.h"

struct Array* array2(uint8_t a, uint8_t b)
{
    struct Array *p = malloc(sizeof(struct Array));// The purpose of array2 is 
                                                   //to create a new Array structure with
                                                   // an initial capacity of 3 and assign 
                                                   //the first two elements to a and b. 
    initArray(p, 3);
    p->array[0] = a;//Assigns a and b to the first two positions in p->array.
    p->array[1] = b;
    p->used = 2;//Sets the used property to 2, indicating that two elements have been assigned
    return p;//Returns the pointer p to the new Array structure.
}

/*Generate an irreducible generator polynomial (necessary to encode a message into Reed-Solomon)*/
struct Array* rs_generator_poly(uint8_t nsym, struct gf_tables *gf_table) // we will not need this as ours is constant.
{
  struct Array *g = malloc(sizeof(struct Array));
  initArray(g, nsym); 
  g->array[0] = 1; 
  insertArray(g); 
  for(uint8_t i = 0; i < nsym; i++)
    g = gf_poly_mul(g, array2(1, gf_pow(2, i, gf_table)), gf_table);
  return g;
}

/*Reed-Solomon main encoding function*/
struct Array* rs_encode_msg(struct Array* msg_in, uint8_t nsym, struct gf_tables *gf_table)// input message, a specified number of error-correction 
                                                                                //symbols (nsym), and a table of Galois Field operations (gf_table)
{
  if(msg_in->used + nsym > 255){ // checks msg length
    fprintf(stderr, "Message too long, %lu is the size when 255 is the max", msg_in->used + nsym);
    exit(EXIT_FAILURE);
  }
  size_t len_gen = nsym; //chaning 
  struct Array *gen = malloc(sizeof(struct Array));
  initArray(gen, len_gen);

  gen = rs_generator_poly(nsym, gf_table); //Generates the Generator Polynomial (we will not need this as our generator polynomial is same)

  struct Array *msg_out = malloc(sizeof(struct Array));//Initialize Output Message Array

  initZArray(msg_out, msg_in->used + gen->used -1);//Initializes msg_out with a length of msg_in->used + gen->used - 1, 
                                                  //which is the combined length of the input message and the required error symbols
  memmove(msg_out->array, msg_in->array, msg_in->used); //Copies the input message msg_in into the start of msg_out

// actual encoding begins here:
  for(size_t i = 0; i < msg_in->used; i++){
    uint8_t coef = msg_out->array[i];
    if (coef != 0){
      for(size_t j = 1; j < gen->used; j++){// j=1 or 0
        msg_out->array[i+j] ^= gf_mul(gen->array[j], coef, gf_table);
      }
    }
  }
  msg_out->used = gen->used + msg_in->used-1;

  memmove(msg_out->array, msg_in->array, msg_in->used); // ask???
  for(size_t i=0;i<gen->used;i++){
    printf("%d  ",gen->array[i]);
  }
  freeArray(gen);
  printf("\n");
  return msg_out;

}
