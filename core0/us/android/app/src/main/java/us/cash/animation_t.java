//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the GPLv3 License as published by the Free
//===-    Software Foundation.
//===-
//===-    This program is distributed in the hope that it will be useful,
//===-    but WITHOUT ANY WARRANTY; without even the implied warranty of
//===-    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//===-
//===-    You should have received a copy of the General Public License
//===-    along with this program, see LICENCE file.
//===-    see https://www.gnu.org/licenses
//===-
//===----------------------------------------------------------------------------
//===-
package us.cash;
import android.graphics.drawable.BitmapDrawable;                                               // BitmapDrawable
import android.graphics.Bitmap;                                                                // Bitmap
import android.graphics.Canvas;                                                                // Canvas
import android.graphics.drawable.Drawable;                                                     // Drawable
import android.graphics.drawable.DrawableWrapper;                                              // DrawableWrapper
import android.graphics.Rect;                                                                  // Rect
import android.content.res.Resources;                                                          // Resources
import android.graphics.drawable.VectorDrawable;                                               // VectorDrawable

public class animation_t extends DrawableWrapper {

    private float rotation;
    private Rect bounds;

    public animation_t(Resources resources, Drawable drawable) {
        super(rasterize(resources, drawable));
        bounds = new Rect();
    }

    @Override
    public void draw(Canvas canvas) {
        copyBounds(bounds);
        canvas.save();
        canvas.rotate(rotation, bounds.centerX(), bounds.centerY());
        super.draw(canvas);
        canvas.restore();
    }

    public void set_rotation(float degrees) {
        this.rotation = degrees % 360;
        invalidateSelf();
    }

    private static Drawable rasterize(Resources resources, Drawable drawable) {
        if (drawable instanceof VectorDrawable) {
            Bitmap b = Bitmap.createBitmap(drawable.getIntrinsicWidth(), drawable.getIntrinsicHeight(), Bitmap.Config.ARGB_8888);
            Canvas c = new Canvas(b);
            drawable.setBounds(0, 0, c.getWidth(), c.getHeight());
            drawable.draw(c);
            drawable = new BitmapDrawable(resources, b);
        }
        return drawable;
    }
}

