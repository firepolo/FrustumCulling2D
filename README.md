# Frustum Culling 2D
Small project for test frustum culling with sfml for graphics lib.

![alt tag](https://raw.githubusercontent.com/firepolo/FrustumCulling2D/master/preview.png)

## Description
The frustum culling is simply render only objects in the frustum view.
Here le red squares are rendered.

![alt tag](https://raw.githubusercontent.com/firepolo/FrustumCulling2D/master/frustum.png)

## How to do ?

### Frustum important data
In the frustum class, there is some constants to see.

```c++
struct Frustum
{
	const float Far = 50.0f;
	const float Fov = glm::pi<float>() / 180.0f * 70.0f;
	const float Fov2 = Fov / 2.0f;
	const float Side = glm::tan(Fov2) * Far;
}
```
Here a scheme for explain the calcul from constant "Side".

![alt tag](https://raw.githubusercontent.com/firepolo/FrustumCulling2D/master/constants.png)

For refresh here also a scheme for function "tan(x)"

![alt tag](https://raw.githubusercontent.com/firepolo/FrustumCulling2D/master/tan.png)

### Create planes
For create planes we must find the target point for calcul the other points.

```c++
target = position + look * Far;

glm::vec2 l = target + glm::vec2(look.y, -look.x) * Side; // Left point
glm::vec2 r = target + glm::vec2(-look.y, look.x) * Side; // Right point
planes[0].set(position, l - position);
planes[1].set(l, r - l);
planes[2].set(r, position - r);
```

Here scheme for explain that

![alt tag](https://raw.githubusercontent.com/firepolo/FrustumCulling2D/master/points.png)

### Check collision
Here for check the collision we use the same algorithm that "polygon vs point" collision.
We apply the dot product with normal from each plane and the relative vector between the point and each origin plane.

```c++
inline bool inner(float x, float y) // Point to check
{
	// Loop on each plane
	for (int i = 0; i < NPlanes; ++i)
	{
		Plane &plane = planes[i];
		if (plane.normal.x * (x - plane.origin.x) + plane.normal.y * (y - plane.origin.y) > 0) return false;
	}
	return true;
}
```
In this code the point is outer if the dot product is greater 0 (return false);
If all dot product return a number lower or equals to 0, also the point is in the polygon.

Scheme for show normals from ABC planes.

![alt tag](https://raw.githubusercontent.com/firepolo/FrustumCulling2D/master/normals.png)

Scheme for show the two way inner and outer.

![alt tag](https://raw.githubusercontent.com/firepolo/FrustumCulling2D/master/collision.png)
Note that arrow black are the normal from ABC planes.

## References
- https://en.wikipedia.org/wiki/Viewing_frustum

## Conclusion
Thanks for reading ;)