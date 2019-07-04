/******************************************************************************
* Projekt - Zaklady pocitacove grafiky - IZG
* Tema:	 Animace na bazi vrcholu a texturovani
* Autor: Klara Necasova
* Email: xnecas24@fit.vutbr.cz
* Datum: Duben 2016
* $Id:xnecas24
*/


/******************************************************************************
* Projekt - Zaklady pocitacove grafiky - IZG
* spanel@fit.vutbr.cz
*
* $Id:$
*/
#include "student.h"
#include "transform.h"
#include "fragment.h"

#include <memory.h>
#include <math.h>


/*****************************************************************************
* Globalni promenne a konstanty
*/

/* Typ/ID rendereru (nemenit) */
const int           STUDENT_RENDERER = 1;

/* Celkova doba behu programu - vyuzito pro interpolaci klicovych snimku */
float totalRunTime = 0.0;

/* Definice vlastniho meterialu  - bila barva */
const S_Material    MAT_WHITE_AMBIENT = { 1.0, 1.0, 1.0, 1.0 };
const S_Material    MAT_WHITE_DIFFUSE = { 1.0, 1.0, 1.0, 1.0 };
const S_Material    MAT_WHITE_SPECULAR = { 1.0, 1.0, 1.0, 1.0 };


/*****************************************************************************
* Funkce vytvori vas renderer a nainicializuje jej
*/

S_Renderer * studrenCreate()
{
	S_StudentRenderer * renderer = (S_StudentRenderer *)malloc(sizeof(S_StudentRenderer));
	IZG_CHECK(renderer, "Cannot allocate enough memory");

	/* inicializace default rendereru */
	renderer->base.type = STUDENT_RENDERER;
	renInit(&renderer->base);

	/* nastaveni ukazatelu na upravene funkce */
	renderer->base.releaseFunc = studrenRelease;
	renderer->base.projectTriangleFunc = studrenProjectTriangle;

	/* inicializace nove pridanych casti */
	/* funkce loadBitmap() vraci pole S_RGBA barev pixelu a take rozmery textury */
	renderer->active_texture = loadBitmap(TEXTURE_FILENAME, &(renderer->texture_width), &(renderer->texture_heigth));
	return (S_Renderer *)renderer;
}

/*****************************************************************************
* Funkce korektne zrusi renderer a uvolni pamet
*/

void studrenRelease(S_Renderer **ppRenderer)
{
	S_StudentRenderer * renderer;

	if (ppRenderer && *ppRenderer)
	{
		/* ukazatel na studentsky renderer */
		renderer = (S_StudentRenderer *)(*ppRenderer);

		/* pripadne uvolneni pameti */
		free(renderer->active_texture);

		/* fce default rendereru */
		renRelease(ppRenderer);
	}
}

/******************************************************************************
* Nova fce pro rasterizaci trojuhelniku s podporou texturovani
* Upravte tak, aby se trojuhelnik kreslil s texturami
* (doplnte i potrebne parametry funkce - texturovaci souradnice, ...)
* v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
* n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
* x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky
*/

void studrenDrawTriangle(S_Renderer *pRenderer,
	S_Coords *v1, S_Coords *v2, S_Coords *v3,
	S_Coords *n1, S_Coords *n2, S_Coords *n3,
	int x1, int y1,
	int x2, int y2,
	int x3, int y3,
	S_Coords  * triangleCoords,
	double hCoord1, double hCoord2, double hCoord3
)
{
	int         minx, miny, maxx, maxy;
	int         a1, a2, a3, b1, b2, b3, c1, c2, c3;
	int         s1, s2, s3;
	int         x, y, e1, e2, e3;
	double      alpha, beta, gamma, w1, w2, w3, z;
	S_RGBA      col1, col2, col3, color;

	double interText_x;
	double interText_y;

	S_RGBA textureColor;

	IZG_ASSERT(pRenderer && v1 && v2 && v3 && n1 && n2 && n3);

	/* vypocet barev ve vrcholech */
	col1 = pRenderer->calcReflectanceFunc(pRenderer, v1, n1);
	col2 = pRenderer->calcReflectanceFunc(pRenderer, v2, n2);
	col3 = pRenderer->calcReflectanceFunc(pRenderer, v3, n3);

	/* obalka trojuhleniku */
	minx = MIN(x1, MIN(x2, x3));
	maxx = MAX(x1, MAX(x2, x3));
	miny = MIN(y1, MIN(y2, y3));
	maxy = MAX(y1, MAX(y2, y3));

	/* oriznuti podle rozmeru okna */
	miny = MAX(miny, 0);
	maxy = MIN(maxy, pRenderer->frame_h - 1);
	minx = MAX(minx, 0);
	maxx = MIN(maxx, pRenderer->frame_w - 1);

	/* Pineduv alg. rasterizace troj.
	hranova fce je obecna rovnice primky Ax + By + C = 0
	primku prochazejici body (x1, y1) a (x2, y2) urcime jako
	(y1 - y2)x + (x2 - x1)y + x1y2 - x2y1 = 0 */

	/* normala primek - vektor kolmy k vektoru mezi dvema vrcholy, tedy (-dy, dx) */
	a1 = y1 - y2;
	a2 = y2 - y3;
	a3 = y3 - y1;
	b1 = x2 - x1;
	b2 = x3 - x2;
	b3 = x1 - x3;

	/* koeficient C */
	c1 = x1 * y2 - x2 * y1;
	c2 = x2 * y3 - x3 * y2;
	c3 = x3 * y1 - x1 * y3;

	/* vypocet hranove fce (vzdalenost od primky) pro protejsi body */
	s1 = a1 * x3 + b1 * y3 + c1;
	s2 = a2 * x1 + b2 * y1 + c2;
	s3 = a3 * x2 + b3 * y2 + c3;

	if (!s1 || !s2 || !s3)
	{
		return;
	}

	/* normalizace, aby vzdalenost od primky byla kladna uvnitr trojuhelniku */
	if (s1 < 0)
	{
		a1 *= -1;
		b1 *= -1;
		c1 *= -1;
	}
	if (s2 < 0)
	{
		a2 *= -1;
		b2 *= -1;
		c2 *= -1;
	}
	if (s3 < 0)
	{
		a3 *= -1;
		b3 *= -1;
		c3 *= -1;
	}

	/* koeficienty pro barycentricke souradnice */
	alpha = 1.0 / ABS(s2);
	beta = 1.0 / ABS(s3);
	gamma = 1.0 / ABS(s1);

	/* vyplnovani... */
	for (y = miny; y <= maxy; ++y)
	{
		/* inicilizace hranove fce v bode (minx, y) */
		e1 = a1 * minx + b1 * y + c1;
		e2 = a2 * minx + b2 * y + c2;
		e3 = a3 * minx + b3 * y + c3;

		for (x = minx; x <= maxx; ++x)
		{
			if (e1 >= 0 && e2 >= 0 && e3 >= 0)
			{
				/* interpolace pomoci barycentrickych souradnic
				e1, e2, e3 je aktualni vzdalenost bodu (x, y) od primek */
				w1 = alpha * e2;
				w2 = beta * e3;
				w3 = gamma * e1;

				/* interpolace z-souradnice */
				z = w1 * v1->z + w2 * v2->z + w3 * v3->z;

				/* interpolace texturovacich souradnic */
				// interText_x = w1 * triangleCoords[0].x + w2 * triangleCoords[1].x + w3 * triangleCoords[2].x;
				// interText_y = w1 * triangleCoords[0].y + w2 * triangleCoords[1].y + w3 * triangleCoords[2].y;

				interText_x = (w1 * triangleCoords[0].x / hCoord1 + w2 * triangleCoords[1].x / hCoord2 + w3 * triangleCoords[2].x / hCoord3) / (w1 / hCoord1 + w2 / hCoord2 + w3 / hCoord3);
				interText_y = (w1 * triangleCoords[0].y / hCoord1 + w2 * triangleCoords[1].y / hCoord2 + w3 * triangleCoords[2].y / hCoord3) / (w1 / hCoord1 + w2 / hCoord2 + w3 / hCoord3);

				/* vypocet barvy textury */
				textureColor = studrenTextureValue((S_StudentRenderer*)pRenderer, interText_x, interText_y);

				/* interpolace barvy */
				color.red = ROUND2BYTE(w1 * col1.red + w2 * col2.red + w3 * col3.red);
				color.green = ROUND2BYTE(w1 * col1.green + w2 * col2.green + w3 * col3.green);
				color.blue = ROUND2BYTE(w1 * col1.blue + w2 * col2.blue + w3 * col3.blue);
				color.alpha = 255;

				/* barvu z osvetlovaciho modelu zkombinujeme s barvou z textury*/
				color.red = color.red * textureColor.red / 255;
				color.green = color.green * textureColor.green / 255;
				color.blue = color.blue * textureColor.blue / 255;

				/* vykresleni bodu */
				if (z < DEPTH(pRenderer, x, y))
				{
					PIXEL(pRenderer, x, y) = color;
					DEPTH(pRenderer, x, y) = z;
				}
			}

			/* hranova fce o pixel vedle */
			e1 += a1;
			e2 += a2;
			e3 += a3;
		}
	}
}

/******************************************************************************
* Vykresli i-ty trojuhelnik n-teho klicoveho snimku modelu
* pomoci nove fce studrenDrawTriangle()
* Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
* aktualne nastavene transformacni matice!
* Upravte tak, aby se model vykreslil interpolovane dle parametru n
* (cela cast n udava klicovy snimek, desetinna cast n parametr interpolace
* mezi snimkem n a n + 1)
* i - index trojuhelniku
* n - index klicoveho snimku (float pro pozdejsi interpolaci mezi snimky)
*/

void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i, float n)
{
	S_Coords	a, b, c;				/* souradnice interpolovanych vrcholu pred transformaci */
	S_Coords    aa, bb, cc;             /* souradnice vrcholu po transformaci */

	S_Coords	na, nb, nc;				/* interpolovane normaly ve vrcholech pred transformaci */
	S_Coords    naa, nbb, ncc;          /* normaly ve vrcholech po transformaci */

	S_Coords	n0;						/* interpolovana normala trojuhelniku pred transformaci */
	S_Coords    nn;                     /* normala trojuhelniku po transformaci */

	int         u1, v1, u2, v2, u3, v3; /* souradnice vrcholu po projekci do roviny obrazovky */
	S_Triangle  * triangle;				/* struktura pro ulozeni i-teho trojuhelniku */

	int         vertexOffset, normalOffset; /* offset pro vrcholy a normalove vektory trojuhelniku */
	int         i0, i1, i2, in;             /* indexy vrcholu a normaly pro i-ty trojuhelnik n-teho snimku */

	int			vertexOffset_1, normalOffset_1; /* offset pro vrcholy a normalove vektory trojuhelniku (n+1)-teho snimku */
	int         i0_1, i1_1, i2_1, in_1;         /* indexy vrcholu a normaly pro i-ty trojuhelnik (n+1)-teho snimku */

	float frac = n - (int)n; /* interpolace mezi odpovidajicimi si dvojicemi vrcholu a normalovych vektoru
							 na zaklade desetinne casti parametru n */

							 /* homogenni souradnice bodu po projekci */
	double homoCoord1;
	double homoCoord2;
	double homoCoord3;

	IZG_ASSERT(pRenderer && pModel && i >= 0 && i < trivecSize(pModel->triangles) && n >= 0);

	/* z modelu si vytahneme i-ty trojuhelnik */
	triangle = trivecGetPtr(pModel->triangles, i);

	/* ziskame offset pro vrcholy n-teho snimku */
	vertexOffset = (((int)n) % pModel->frames) * pModel->verticesPerFrame;
	/* ziskame offset pro vrcholy (n+1)-teho snimku */
	vertexOffset_1 = (((int)n + 1) % pModel->frames) * pModel->verticesPerFrame;

	/* ziskame offset pro normaly trojuhelniku n-teho snimku */
	normalOffset = (((int)n) % pModel->frames) * pModel->triangles->size;
	/* ziskame offset pro normaly trojuhelniku (n+1)-teho snimku */
	normalOffset_1 = (((int)n + 1) % pModel->frames) * pModel->triangles->size;

	/* indexy vrcholu pro i-ty trojuhelnik n-teho snimku - pricteni offsetu */
	i0 = triangle->v[0] + vertexOffset;
	i1 = triangle->v[1] + vertexOffset;
	i2 = triangle->v[2] + vertexOffset;
	/* indexy vrcholu pro i-ty trojuhelnik (n+1)-teho snimku - pricteni offsetu */
	i0_1 = triangle->v[0] + vertexOffset_1;
	i1_1 = triangle->v[1] + vertexOffset_1;
	i2_1 = triangle->v[2] + vertexOffset_1;

	/* index normaloveho vektoru pro i-ty trojuhelnik n-teho snimku - pricteni offsetu */
	in = triangle->n + normalOffset;
	/* index normaloveho vektoru pro i-ty trojuhelnik (n+1)-teho snimku - pricteni offsetu */
	in_1 = triangle->n + normalOffset_1;

	/* zisk souradnic vrcholu i-teho trojuhelniku n-teho snimku */
	a = *cvecGetPtr(pModel->vertices, i0);
	b = *cvecGetPtr(pModel->vertices, i1);
	c = *cvecGetPtr(pModel->vertices, i2);

	/* zisk souradnic vrcholu i-teho trojuhelniku (n+1)-teho snimku */
	aa = *cvecGetPtr(pModel->vertices, i0_1);
	bb = *cvecGetPtr(pModel->vertices, i1_1);
	cc = *cvecGetPtr(pModel->vertices, i2_1);

	/* interpolace mezi body */
	aa.x = a.x*(1.0 - frac) + aa.x*frac;
	aa.y = a.y*(1.0 - frac) + aa.y*frac;
	aa.z = a.z*(1.0 - frac) + aa.z*frac;

	bb.x = b.x*(1.0 - frac) + bb.x*frac;
	bb.y = b.y*(1.0 - frac) + bb.y*frac;
	bb.z = b.z*(1.0 - frac) + bb.z*frac;

	cc.x = c.x*(1.0 - frac) + cc.x*frac;
	cc.y = c.y*(1.0 - frac) + cc.y*frac;
	cc.z = c.z*(1.0 - frac) + cc.z*frac;


	/* transformace vrcholu matici model */
	trTransformVertex(&a, &aa);
	trTransformVertex(&b, &bb);
	trTransformVertex(&c, &cc);

	/* promitneme vrcholy trojuhelniku na obrazovku */
	homoCoord1 = trProjectVertex(&u1, &v1, &a);
	homoCoord2 = trProjectVertex(&u2, &v2, &b);
	homoCoord3 = trProjectVertex(&u3, &v3, &c);

	/* zisk souradnic normal i-teho trojuhelniku n-teho snimku */
	na = *cvecGetPtr(pModel->normals, i0);
	nb = *cvecGetPtr(pModel->normals, i1);
	nc = *cvecGetPtr(pModel->normals, i2);

	/* zisk souradnic normal i-teho trojuhelniku (n+1)-teho snimku */
	naa = *cvecGetPtr(pModel->normals, i0_1);
	nbb = *cvecGetPtr(pModel->normals, i1_1);
	ncc = *cvecGetPtr(pModel->normals, i2_1);

	/* interpolace normal ve vrcholech */
	naa.x = na.x*(1.0 - frac) + naa.x*frac;
	naa.y = na.y*(1.0 - frac) + naa.y*frac;
	naa.z = na.z*(1.0 - frac) + naa.z*frac;

	nbb.x = nb.x*(1.0 - frac) + nbb.x*frac;
	nbb.y = nb.y*(1.0 - frac) + nbb.y*frac;
	nbb.z = nb.z*(1.0 - frac) + nbb.z*frac;

	ncc.x = nc.x*(1.0 - frac) + ncc.x*frac;
	ncc.y = nc.y*(1.0 - frac) + ncc.y*frac;
	ncc.z = nc.z*(1.0 - frac) + ncc.z*frac;

	/* pro osvetlovaci model transformujeme take normaly ve vrcholech */
	trTransformVector(&na, &naa);
	trTransformVector(&nb, &nbb);
	trTransformVector(&nc, &ncc);

	/* normalizace normal */
	coordsNormalize(&na);
	coordsNormalize(&nb);
	coordsNormalize(&nc);

	/* transformace normaly trojuhelniku matici model */
	n0 = *cvecGetPtr(pModel->trinormals, in);
	nn = *cvecGetPtr(pModel->trinormals, in_1);

	nn.x = n0.x*(1.0 - frac) + nn.x*frac;
	nn.y = n0.y*(1.0 - frac) + nn.y*frac;
	nn.z = n0.z*(1.0 - frac) + nn.z*frac;

	trTransformVector(&n0, &nn);

	/* normalizace normaly */
	coordsNormalize(&n0);

	/* je troj. privraceny ke kamere, tudiz viditelny? */
	if (!renCalcVisibility(pRenderer, &a, &n0))
	{
		/* odvracene troj. vubec nekreslime */
		return;
	}

	/* rasterizace trojuhelniku */
	studrenDrawTriangle(pRenderer,
		&a, &b, &c,
		&na, &nb, &nc,
		u1, v1, u2, v2, u3, v3,
		triangle->t,
		homoCoord1, homoCoord2, homoCoord3
	);
}

/******************************************************************************
* Vraci hodnotu v aktualne nastavene texture na zadanych
* texturovacich souradnicich u, v
* Pro urceni hodnoty pouziva bilinearni interpolaci
* Pro otestovani vraci ve vychozim stavu barevnou sachovnici dle uv souradnic
* u, v - texturovaci souradnice v intervalu 0..1, ktery odpovida sirce/vysce textury
*/

S_RGBA studrenTextureValue(S_StudentRenderer * pRenderer, double u, double v)
{
	/* nalezneme odpovadajici x-ovou a y-ovou souradnici */
	double x = v * (pRenderer->texture_width - 1);
	double y = u * (pRenderer->texture_heigth - 1);

	/* nalezneme 4 nejblizsi pixely dle souradnic u a v */
	/* aktualni pozice pixelu */
	const S_RGBA * const pix1 = pRenderer->active_texture + (int)x + (int)y * pRenderer->texture_width;
	/* pixel napravo od aktualniho pixelu: x + 1 */
	const S_RGBA * const pix2 = pix1 + 1;
	/* pixel pod aktualnim pixelem: y - 1 */
	const S_RGBA * const pix3 = pix1 + pRenderer->texture_width;
	/* pixel diagonalne od aktulniho pixelu: x + 1, y - 1 */
	const S_RGBA * const pix4 = pix1 + pRenderer->texture_width + 1;

	/* desetinna cast souradnice x */
	double fx = x - (int)x;
	/* desetinna cast souradnice y */
	double fy = y - (int)y;

	/* vypocet vah pro jednotlive body */
	int w1 = (int)((1.0 - fx) * (1.0 - fy) * 256.0);
	int w2 = (int)(fx  * (1.0 - fy) * 256.0);
	int w3 = (int)((1.0 - fx) * fy  * 256.0);
	int w4 = (int)(fx  * fy  * 256.0);

	/* vypocitame barvy jednotlivych pixelu */
	int red = (pix1->red   * w1 + pix2->red   * w2 + pix3->red   * w3 + pix4->red * w4) >> 8; // stejne jako /256 
	int green = (pix1->green * w1 + pix2->green * w2 + pix3->green * w3 + pix4->green * w4) >> 8;
	int blue = (pix1->blue  * w1 + pix2->blue  * w2 + pix3->blue  * w3 + pix4->blue * w4) >> 8;
	int alpha = (pix1->alpha * w1 + pix2->alpha * w2 + pix3->alpha * w3 + pix4->alpha * w4) >> 8;

	/* vytvorime barvu */
	return makeColorA(red, green, blue, alpha);

	// unsigned char c = ROUND2BYTE( ( ( fmod( u * 10.0, 1.0 ) > 0.5 ) ^ ( fmod( v * 10.0, 1.0 ) < 0.5 ) ) * 255 );
	// return makeColor( c, 255 - c, 0 );
}

/******************************************************************************
******************************************************************************
* Funkce pro vyrenderovani sceny, tj. vykresleni modelu
* Upravte tak, aby se model vykreslil animovane
* (volani renderModel s aktualizovanym parametrem n)
*/

void renderStudentScene(S_Renderer *pRenderer, S_Model *pModel)
{
	/* test existence frame bufferu a modelu */
	IZG_ASSERT(pModel && pRenderer);

	/* nastavit projekcni matici */
	trProjectionPerspective(pRenderer->camera_dist, pRenderer->frame_w, pRenderer->frame_h);

	/* vycistit model matici */
	trLoadIdentity();

	/* nejprve nastavime posuv cele sceny od/ke kamere */
	trTranslate(0.0, 0.0, pRenderer->scene_move_z);

	/* nejprve nastavime posuv cele sceny v rovine XY */
	trTranslate(pRenderer->scene_move_x, pRenderer->scene_move_y, 0.0);

	/* natoceni cele sceny - jen ve dvou smerech - mys je jen 2D... :( */
	trRotateX(pRenderer->scene_rot_x);
	trRotateY(pRenderer->scene_rot_y);

	/* nastavime material */
	renMatAmbient(pRenderer, &MAT_WHITE_AMBIENT);
	renMatDiffuse(pRenderer, &MAT_WHITE_DIFFUSE);
	renMatSpecular(pRenderer, &MAT_WHITE_SPECULAR);

	/* a vykreslime nas model (ve vychozim stavu kreslime pouze snimek 0) */
	renderModel(pRenderer, pModel, totalRunTime);
}

/* Callback funkce volana pri tiknuti casovace
* ticks - pocet milisekund od inicializace */
void onTimer(int ticks)
{
	/* uprava parametru pouzivaneho pro vyber klicoveho snimku
	* a pro interpolaci mezi snimky */

	// Vypocitame celkovou dobu behu programu 
	// tato hodnota se pouzije pro interpolaci klicovych snimku 

	//static int previous_ticks = 0;
	//totalRunTime += (float)((ticks - previous_ticks) / 100.0);
	//previous_ticks = ticks;

	totalRunTime = (float)(ticks / 100.0);
	return;
}

/*****************************************************************************
*****************************************************************************/
