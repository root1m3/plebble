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
package com.jstyle.blesdk2025.Util;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.graphics.pdf.PdfDocument;
import android.text.TextUtils;

import com.jstyle.blesdk2025.model.UserInfo;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;

public class PDFCreate {
    private static final String TAG = "PDFCreate";
    final static int maxXSize = 5120;
      static float  length = 20;
     static float totalWidth = 1070;
     static int startX = 35;
     static float endX = startX + 1000;
     static float startY ;
    private static float endY;
    private  static float height ;
    private  static float width ;
    private static int strokeWidthTime;
    private static int strokeWidthLine;
    private static float totalHeight;

    public static void createPdf(String path, Context context, List<Integer> data, UserInfo userInfo) {
        int size = data.size();
        float col = size % maxXSize == 0 ? size / maxXSize : size / maxXSize + 1;//一次画5120个点，分多少次画
        length=dip2px(context,10);
        height=length;
        width=length;
        startX=dip2px(context,20);
        endX=startX+50*length;
        totalWidth=endX+startX;
        totalHeight = col * height * 5 + dip2px(context,130);//一次需要5个高度
        PdfDocument pdfDocument = new PdfDocument();

        PdfDocument.PageInfo pageInfo = new PdfDocument.PageInfo.Builder((int) totalWidth, (int) totalHeight, 1)
                // .setContentRect(new Rect(0,0,(int)totalWidth,(int)totalHeight))
                .create();

        PdfDocument.Page page = pdfDocument.startPage(pageInfo);
        Canvas canvas = page.getCanvas();
        Paint paint = new Paint();
        strokeWidthTime = dip2px(context, 1.5f);
        strokeWidthLine = dip2px(context, 0.5f);
        drawReportInfo(context, canvas, paint,userInfo);
        Path pathCanvas = new Path();
        endY = startY + col * height * 5;
        paint.setTextSize(dip2px(context, 15));
        drawAxes(pathCanvas, canvas, paint, col);
        drawTimeLine(pathCanvas, canvas, paint);
        drawDataLine(context,pathCanvas, canvas, paint, col, data);
        pdfDocument.finishPage(page);
        File file = new File(path);
        FileOutputStream outputStream = null;
        try {
            outputStream = new FileOutputStream(file);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        try {
            pdfDocument.writeTo(outputStream);
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    /**用户信息
     * @param context
     * @param canvas
     * @param paint
     * @param
     */
    private static void drawReportInfo(Context context, Canvas canvas, Paint paint,UserInfo userInfo) {
        paint.setTextSize(dip2px(context,15));
        paint.setColor(Color.BLACK);
        paint.setStyle(Paint.Style.FILL);
        if(null!=userInfo){
            if(!TextUtils.isEmpty(userInfo.getEcgTitle())){
                String title=userInfo.getEcgTitle();
                Rect rect = new Rect();
                paint.getTextBounds(title, 0, title.length(), rect);
                int heightTitle = rect.height();
                float startTextY=heightTitle+dip2px(context,8);
                float marginTop=dip2px(context,4);
                float marginRight=dip2px(context,8);
                //标题
                canvas.drawText(title, totalWidth / 2 - 150, startTextY, paint);
                //提醒
                paint.setTextSize(dip2px(context, 12));
                int widthTips = rect.width();
                int heightTips = rect.height();
                if(!TextUtils.isEmpty(userInfo.getEcgReportTips())) {

                    String tips = userInfo.getEcgReportTips();
                    float tipsTextY = startTextY + heightTitle + marginTop;
                    paint.getTextBounds(tips, 0, tips.length(), rect);
                    canvas.drawText(tips, totalWidth - widthTips - marginRight, tipsTextY, paint);
                }

                float tipsTextY=startTextY+heightTitle+marginTop;


                String name = userInfo.getName();
                String genderString = userInfo.getGender();
                String age = userInfo.getAge();


                String userWeight = userInfo.getWeight();
                String userHeight = userInfo.getHeight();
                //个人信息
                float infoTextY=tipsTextY+heightTips+marginTop;
                @SuppressLint("StringFormatMatches")
                String info=name+"  "+genderString+"  "+age+"  "+ userHeight+"  "+userWeight;
                paint.getTextBounds(info, 0, info.length(), rect);
                int heightInfo = rect.height();
                int widthInfo = rect.width();
                canvas.drawText(info, totalWidth / 2 - (widthInfo >> 1), infoTextY, paint);
                //时间
                float dateTextY=infoTextY+heightInfo+marginTop;
                String dateString=userInfo.getDate();
                paint.getTextBounds(dateString, 0, dateString.length(), rect);
                int widthDate=rect.width();
                int heightDate=rect.height();
                canvas.drawText(dateString, totalWidth - widthDate-marginRight, dateTextY, paint);
                startY=dateTextY+heightDate+marginTop;
            }




        }

    }

    /**
     * 画网格
     *
     * @param pathCanvas
     * @param canvas
     * @param paint
     * @param col
     */
    private static void drawAxes(Path pathCanvas, Canvas canvas, Paint paint, float col) {
        //画网格竖线
        int colorLine = Color.rgb(243, 119, 99);
        paint.setColor(colorLine);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(strokeWidthLine);
        paint.setAlpha(200);
        for (int i = 0; i <51; i++) {
            pathCanvas.moveTo(i * width + startX, startY);
            pathCanvas.lineTo(i * width + startX, endY);
        }
        //画网格横线
        for (int i = 0; i <= col * 5; i++) {
            pathCanvas.moveTo(startX - (strokeWidthTime >> 1), i * height + startY);
            pathCanvas.lineTo(endX + (strokeWidthTime >> 1), i * height + startY);
        }
        canvas.drawPath(pathCanvas, paint);
        pathCanvas.reset();
    }

    /**
     * 加粗时间线
     *
     * @param pathCanvas
     * @param canvas
     * @param paint
     */
    private static void drawTimeLine(Path pathCanvas, Canvas canvas, Paint paint) {
        paint.setColor(Color.BLACK);
        paint.setStrokeWidth(strokeWidthTime);
        paint.setStyle(Paint.Style.FILL);
        Rect rect=new Rect();
        for (int i = 0; i < 11; i++) {
            pathCanvas.moveTo(i * 5 * width + startX, startY);
            pathCanvas.lineTo(i * 5 * width + startX, endY + 20);
            String time=i+"s";
            paint.getTextBounds(time, 0, time.length(), rect);
            int widthTime=rect.width();
            canvas.drawText(time, i * 5 * width + startX - (widthTime >> 1), endY + 50, paint);
        }
        int colorS = Color.rgb(255, 119, 99);
        paint.setColor(colorS);
        paint.setAlpha(255);
        paint.setStyle(Paint.Style.STROKE);
        canvas.drawPath(pathCanvas, paint);
        pathCanvas.reset();
    }

    /**
     * 画数据
     *
     * @param pathCanvas
     * @param canvas
     * @param paint
     * @param col
     * @param data
     */
    private static void drawDataLine(Context context,Path pathCanvas, Canvas canvas, Paint paint, float col, List<Integer> data) {
        int size = data.size();
        paint.setStrokeWidth(strokeWidthLine);
        paint.setColor(Color.BLACK);
        out:
        for (int i = 0; i < col; i++) {
            int startPoint = i * maxXSize;
           in: for (int j = 0; j <maxXSize; j++) {
                int x = j;
                if (startPoint + j >= size) break out;
               Integer y = data.get(startPoint + j);
                if (j == 0) pathCanvas.moveTo(startX, i * height * 5 + getCanvasY(y,context));
                pathCanvas.lineTo(startX + x * (endX - startX) / maxXSize, i * height * 5 + getCanvasY(y,context));
            }
        }

        canvas.drawPath(pathCanvas, paint);
    }

    private static float getCanvasY(double value,Context context) {
        double height = 5 * 10 / 16000f;
        return  dip2px (context,(float) (height * (8000 - value))) + startY;
    }

    /**
     * 根据手机的分辨率从 dp 的单位 转成为 px(像素)
     */
    public static int dip2px(Context context, float dpValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }
}
