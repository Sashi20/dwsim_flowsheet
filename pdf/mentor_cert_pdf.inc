<?php
function generate_pdf()
{
	$mpath = drupal_get_path('module', 'dwsim_flowsheet');
	require($mpath . '/pdf/fpdf/fpdf.php');
	require($mpath . '/pdf/phpqrcode/qrlib.php');
	global $user;
	$x                 = $user->uid;
	$proposal_id       = arg(3);
	$query3 = db_query("SELECT * FROM dwsim_flowsheet_proposal WHERE approval_status=3 AND id=:proposal_id", array(
		':proposal_id'=>$proposal_id
	));
	$data3             = $query3->fetchObject();
	$gender            = array(
		'salutation' => 'Mr. /Ms.',
		'gender' => 'He/She'
	);
	if ($data3->gender) {
		if ($data3->gender == 'M') {
			$gender = array(
				'salutation' => 'Mr.',
				'gender' => 'He'
			);
		} //$data3->gender == 'M'
		else {
			$gender = array(
				'salutation' => 'Ms.',
				'gender' => 'She'
			);
		}
	} //$data3->gender
	$pdf = new FPDF('L', 'mm', 'Letter');
	if (!$pdf) {
		echo "Error!";
	} //!$pdf
	$pdf->AddPage();
	$image_bg = $mpath . "/pdf/images/bg_cert_mentor.png";
	$pdf->Image($image_bg, 0, 0, $pdf->GetPageWidth(), $pdf->GetPageHeight());
	$pdf->SetMargins(18, 1, 18);
	$path   = drupal_get_path('module', 'dwsim_flowsheet');
	$pdf->Ln(15);
	$pdf->Ln(43);
	$pdf->SetFont('Times', 'I', 12);
	$pdf->SetTextColor(13, 5, 130);
	$pdf->Cell(240, 20, 'This certificate recognizes the valuable mentorship of', '0', '1', 'C');
	$pdf->Ln(-6);
	$pdf->SetFont('Times', 'BI', 16);
	$pdf->SetTextColor(13, 5, 130);
	$pdf->Cell(240, 8, utf8_decode($data3->project_guide_name), '0', '1', 'C');
	$pdf->Ln(0);
	$pdf->SetFont('Times', 'I', 12);
	if (strtolower($data3->branch) != "others") {
		$pdf->SetTextColor(13, 5, 130);
		$pdf->SetFont('Times', 'I', 12);
		$pdf->Cell(240, 8, 'from', '0', '1', 'C');
		$pdf->Ln(0);
		$pdf->SetFont('Times','BI',16);
		$pdf->Cell(240, 8, utf8_decode($data3->project_guide_university), '0','1','C');
		$pdf->Ln(0);
		$pdf->SetFont('Times', 'I', 12);
		$pdf->Cell(240, 8, 'who has mentored', '0', '1', 'C');
		$pdf->Ln(0);
		$pdf->SetFont('Times', 'BI', 16);
		$pdf->Cell(240, 8, utf8_decode($data3->contributor_name), '0', '1', 'C');
		$pdf->Ln(0);
		$pdf->SetFont('Times', 'I', 12);
		$pdf->Cell(240, 8, 'for successfully completing internship under the DWSIM Flowsheeting project. The intern(s) has created a flowsheet titled', '0', '1', 'C');
		$pdf->Ln(0);
		$pdf->SetFont('Times', 'BI', 16);
		$pdf->SetTextColor(13, 5, 130);
		$pdf->Cell(240, 8, utf8_decode($data3->project_title), '0', '1', 'C');
		$pdf->SetTextColor(13, 5, 130);
		$pdf->Ln(0);
		$pdf->SetFont('Times', 'I', 12);
		$pdf->Cell(240, 8, ' using DWSIM .The work done is available at', '0', '1', 'C');
		$pdf->Cell(240, 4, '', '0', '1', 'C');
		$pdf->SetX(120);
		$pdf->SetFont('', 'U');
		$pdf->SetTextColor(13, 5, 130);
		$pdf->SetFont('Times', 'I', 12);
		$pdf->write(0, 'https://dwsim.fossee.in/', 'https://dwsim.fossee.in/');
		$pdf->Ln(0);
	} 
	else {
		$pdf->SetTextColor(13, 5, 130);
		$pdf->Cell(240, 8, 'from ' . $data3->project_guide_university . ' has successfully', '0', '1', 'C');
		$pdf->Ln(0);
		$pdf->Cell(240, 8, 'completed Internship under DWSIM Flowsheeting Project', '0', '1', 'C');
		$pdf->Ln(0);
	}
	$proposal_get_id = 0;
	$UniqueString    = "";
	$tempDir         = $path . "/pdf/temp_prcode/";
	$query           = db_select('dwsim_flowsheet_qr_code');
	$query->fields('dwsim_flowsheet_qr_code');
	$query->condition('proposal_id', $proposal_id);
	$result          = $query->execute();
	$data            = $result->fetchObject();
	$proposal_get_id = $data->proposal_id;
	$qrstring        = $data->qr_code;
	$codeContents        = 'https://dwsim.fossee.in/flowsheeting-project/certificates/verify/' . $qrstring;
	$fileName            = 'generated_qrcode.png';
	$pngAbsoluteFilePath = $tempDir . $fileName;
	$urlRelativeFilePath = $path . "/pdf/temp_prcode/" . $fileName;
	QRcode::png($codeContents, $pngAbsoluteFilePath);
	$pdf->SetFont('Times', 'B', 12);
	$pdf->SetY(-58);
	$pdf->Ln(13);
	$pdf->SetX(10);
	$pdf->Cell(0, 0, $qrstring, 0, 0, 'C');
	$pdf->SetX(29);
	$pdf->SetY(-50);
	$pdf->Ln(13);
	$pdf->Image($pngAbsoluteFilePath, $pdf->GetX() + 102, $pdf->GetY() - 5, 30, 0);
	$filename = str_replace(' ', '-', $data3->contributor_name) . '-DWSIM-Flowsheet-Certificate.pdf';
	$file     = $path . '/pdf/temp_certificate/' . $proposal_id . '_' . $filename;
	$pdf->Output($file, 'F');
	header("Content-Type: application/octet-stream");
	header("Content-Disposition: attachment; filename=" . $filename);
	header("Content-Type: application/octet-stream");
	header("Content-Type: application/download");
	header("Content-Description: File Transfer");
	header("Content-Length: " . filesize($file));
	flush();
	$fp = fopen($file, "r");
	while (!feof($fp)) {
		echo fread($fp, 65536);
		flush();
	} //!feof($fp)
	fclose($fp);
	unlink($file);
	//drupal_goto('flowsheeting-project/certificate');
	return;
}
function generateRandomString($length = 5)
{
	$characters       = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
	$charactersLength = strlen($characters);
	$randomString     = '';
	for ($i = 0; $i < $length; $i++) {
		$randomString .= $characters[rand(0, $charactersLength - 1)];
	} //$i = 0; $i < $length; $i++
	return $randomString;
}
function generate_copyright_form_pdf()
{
	$mpath = drupal_get_path('module', 'dwsim_flowsheet');
	require($mpath . '/pdf/fpdf/fpdf.php');
	global $user;
	$x           = $user->uid;
	$proposal_id = arg(3);
	$query2 = db_query("SELECT id FROM dwsim_flowsheet_proposal WHERE approval_status=3 AND uid= :uid", array(
		':uid' => $user->uid
	));
	$data2             = $query2->fetchObject();
	$query3 = db_query("SELECT * FROM dwsim_flowsheet_proposal WHERE approval_status=3 AND uid= :uid", array(
		':uid' => $user->uid
	));
	$data3       = $query3->fetchObject();
	$gender      = array(
		'salutation' => 'Mr. /Ms.',
		'gender' => 'He/She'
	);
	if ($data3->gender) {
		if ($data3->gender == 'M') {
			$gender = array(
				'salutation' => 'Mr.',
				'gender' => 'He'
			);
		} //$data3->gender == 'M'
		else {
			$gender = array(
				'salutation' => 'Ms.',
				'gender' => 'She'
			);
		}
	} //$data3->gender
	$pdf = new FPDF('P', 'mm', 'Letter');
	if (!$pdf) {
		echo "Error!";
	} //!$pdf
	$pdf->AddPage();
	$path = drupal_get_path('module', 'dwsim_flowsheet');
	$pdf->SetFont('Times', 'B', 25);
	$pdf->Ln(30);
	$pdf->Cell(200, 8, 'Copyright Transfer Form', 0, 1, 'C');
	$pdf->Ln(20);
	$pdf->SetFont('Times', '', 12);
	$pdf->MultiCell(200, 8, 'I hereby transfer the copyrights of the DWSIM Flowsheeting Project for ' . $data2->project_title . ' to FOSSEE Project, IIT Bombay.', 0);
	$pdf->Ln(10);
	$pdf->MultiCell(200, 8, 'I  understand  that  the  FOSSEE  project  will  release  the  Flowsheet  under  the Creative Commons (CC) license.');
	$pdf->SetX(75);
	$pdf->SetFont('', 'U');
	$pdf->SetTextColor(0, 0, 255);
	$pdf->SetFont('', '');
	$pdf->SetTextColor(0, 0, 0);
	$pdf->SetFont('', '');
	$pdf->SetTextColor(0, 0, 0);
	$pdf->SetY(-10);
	$pdf->SetX(209);
	$cur_date = date('jS F, Y');
	$pdf->SetY(140);
	$pdf->SetFont('', '');
	$pdf->Ln(0);
	$pdf->Cell(200, 0, ' Date: ' . $cur_date . '', 0, 1, 'L');
	$pdf->Cell(200, 20, ' Place: _________________', 0, 1, 'L');
	$pdf->SetY(140);
	$pdf->SetX(120);
	$pdf->Cell(180, 0, 'Signature: _________________', 0, 1, 'L');
	$pdf->SetY(144);
	$pdf->SetX(120);
	$pdf->multicell(140, 14, 'Name: ' . $gender['salutation'] . ' ' . $data3->full_name . '', 0, '');
	$filename = str_replace(' ', '-', $data3->full_name) . '-DWSIM-Flowsheet-copyright-form.pdf';
	$file     = $path . '/pdf/temp_certificate/' . $proposal_id . '_' . $filename;
	$pdf->Output($file, 'F');
	header("Content-Type: application/octet-stream");
	header("Content-Disposition: attachment; filename=" . $filename);
	header("Content-Type: application/octet-stream");
	header("Content-Type: application/download");
	header("Content-Description: File Transfer");
	header("Content-Length: " . filesize($file));
	flush();
	$fp = fopen($file, "r");
	while (!feof($fp)) {
		echo fread($fp, 65536);
		flush();
	} //!feof($fp)
	fclose($fp);
	unlink($file);
	drupal_goto('Summer_Internship_Forms/forms');
	return;
}
function generate_undertaking_form_pdf()
{
	$mpath = drupal_get_path('module', 'textbook_companion');
	require($mpath . '/pdf/fpdf/fpdf.php');
	global $user;
	$x           = $user->uid;
	$proposal_id = arg(3);
	$query2 = db_query("SELECT id FROM dwsim_flowsheet_proposal WHERE approval_status=3 AND uid= :uid", array(
		':uid' => $user->uid
	));
	$data2             = $query2->fetchObject();
	$query3 = db_query("SELECT * FROM dwsim_flowsheet_proposal WHERE approval_status=3 AND uid= :uid", array(
		':uid' => $user->uid
	));
	$data3       = $query3->fetchObject();
	$gender      = array(
		'salutation' => 'Mr. /Ms.',
		'gender' => 'He/She'
	);
	if ($data3->gender) {
		if ($data3->gender == 'M') {
			$gender = array(
				'salutation' => 'Mr.',
				'gender' => 'He'
			);
		} //$data3->gender == 'M'
		else {
			$gender = array(
				'salutation' => 'Ms.',
				'gender' => 'She'
			);
		}
	} //$data3->gender
	$pdf = new FPDF('P', 'mm', 'Letter');
	if (!$pdf) {
		echo "Error!";
	} //!$pdf
	$pdf->AddPage();
	$path = drupal_get_path('module', 'dwsim_flowsheet');
	$pdf->SetFont('Times', 'B', 25);
	$pdf->Ln(30);
	$pdf->Cell(200, 8, 'Undertaking Form', 0, 1, 'C');
	$pdf->Ln(0);
	$pdf->SetFont('Times', 'B', 10);
	$pdf->Cell(200, 8, '(To be signed by college teacher)', 0, 1, 'C');
	$pdf->Ln(20);
	$pdf->SetFont('Times', '', 12);
	$pdf->MultiCell(200, 8, 'I hereby certify that all the codes written by ' . $gender['salutation'] . ' ' . $data3->full_name . ' under the DWSIM Textbook Companion Project for the book ' . $data2->book . ' ( Author: ' . $data2->author . ', Edition: ' . $data2->edition . ', Publisher: ' . $data2->publisher . ', Year: ' . $data2->year . ') are correctly reproducing the results given in the aforementioned book.', 0);
	$pdf->Ln(10);
	$pdf->MultiCell(200, 8, 'I understand that the DWSIM Textbook Companion created is a part of FOSSEE project, IIT Bombay, and is sponsored by the National Mission on Education through Information and Communication Technology (NMEICT), under MHRD, Govt. of India. The project requires that the textbook companion is made available for public access as an open source document. Hence I undertake that this DWSIM Textbook Companion can be made public along with the information that I have certified all the codes as giving the correct answer.');
	$pdf->SetX(75);
	$pdf->SetFont('', 'U');
	$pdf->SetTextColor(0, 0, 255);
	$pdf->SetFont('', '');
	$pdf->SetTextColor(0, 0, 0);
	$pdf->SetFont('', '');
	$pdf->SetTextColor(0, 0, 0);
	$pdf->SetY(-10);
	$pdf->SetX(209);
	$cur_date = date('jS F, Y');
	$pdf->SetY(180);
	$pdf->SetFont('', '');
	$pdf->Ln(0);
	$pdf->Cell(200, 0, ' Date: ' . $cur_date . '', 0, 1, 'L');
	$pdf->Cell(200, 20, ' Place: _________________', 0, 1, 'L');
	$pdf->SetY(180);
	$pdf->SetX(120);
	$pdf->Cell(140, 0, 'Signature: _________________', 0, 1, 'L');
	$pdf->SetX(120);
	$pdf->multicell(140, 14, 'Name: ' . '____________________', 0, '');
	$pdf->SetX(120);
	$pdf->multicell(140, 14, 'Designation: ' . '______________', 0, '');
	$filename = str_replace(' ', '-', $data3->full_name) . '-DWSIM-Textbook-Companion-undertaking-form.pdf';
	$file     = $path . '/pdf/temp_certificate/' . $proposal_id . '_' . $filename;
	$pdf->Output($file, 'F');
	header("Content-Type: application/octet-stream");
	header("Content-Disposition: attachment; filename=" . $filename);
	header("Content-Type: application/octet-stream");
	header("Content-Type: application/download");
	header("Content-Description: File Transfer");
	header("Content-Length: " . filesize($file));
	flush();
	$fp = fopen($file, "r");
	while (!feof($fp)) {
		echo fread($fp, 65536);
		flush();
	} //!feof($fp)
	fclose($fp);
	unlink($file);
	drupal_goto('Summer_Internship_Forms/forms');
	return;
}
